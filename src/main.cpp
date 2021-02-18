#include <algorithm>
#include <iostream>
#include <sstream>
#include <locale>
#include <unicode/unistr.h>

#include <deque>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "configuration.h"
#include "dictionary.h"
#include "spellcheck.h"
#include "threads.h"

inline bool isInteger(const std::string & s){
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

   char * p;
   strtol(s.c_str(), &p, 10);

   return (*p == 0);
}

/****************************************************
Normalize(string) -
Converts string to lowercase and removes punctuation.
*****************************************************/
std::string Normalize(std::string s){
    // Initialize variables
    std::string nString;

    // Convert std::string to ICU's UnicodeString
    icu_68::UnicodeString ucs = icu_68::UnicodeString::fromUTF8(icu_68::StringPiece(s.c_str()));
    ucs.toLower();
    ucs.toUTF8String(nString);

    // Return converted string
    return nString;
}

// Global Queues
std::deque<std::string> g_lines;
std::deque<std::string> g_words[32];

// Global Lock
std::mutex g_mutex_lines, g_mutex_words[32], g_mutex_lines_spell, g_mutex_lines_dict;

// Global Conditional Variables
std::condition_variable g_cond, g_spell, g_dict;

// FileReader Run Marker
bool producer_is_running = true;

// Calculate the worker queue
int calc_queue(int character, int workers){
  if(character > 90){
    return (character - 97) * workers / 27;
  } else {
    return (character - 65) * workers / 27;
  }
}

// FileReader Thread Function
void FileReader(std::string filename){
  int counter = 0;
  std::string line;
  std::ifstream m_InputFile;

  bool consume = false;

  m_InputFile.open(filename);


  while(std::getline(m_InputFile, line)){
    // Smart lock, lock when initialized, protect within code curly brackets, and automatically unlock when curly brackets exit
    // You can manually unlock to control the fine granularity of mutexes
    std::unique_lock<std::mutex> locker(g_mutex_lines);

    counter += 1;

    if(counter % 900 == 0){
      std::cout << counter << '\r' << std::flush;
    }

    // Queue a data
    g_lines.push_back(line);

    if(g_lines.size() > 200){
      consume = true;
    }

    if(consume){
      // Unlock ahead of time, reduce the fine-grained mutex, and synchronize protection only for shared queue data
      locker.unlock();

      // Wake up a thread
      g_cond.notify_one();
      while(g_lines.size() > 50){
        // std::cout << "Deque size: " << g_lines.size() << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
      }

      consume = false;
    }
  }

  m_InputFile.close();

  // Mark FileReader Proofed
  producer_is_running = false;

  // Wake up all consumer threads
  g_cond.notify_all();

  std::cout << "FileReader: Completed reading file" << std::endl;
}

void LineReader(int workers){
  std::string line;
  std::string subs;
  std::string word;

  std::istringstream iss;

  bool consume = false;

  int counter = 0;

  do{
    // Smart lock, lock when initialized, protect within code curly brackets, and automatically unlock when curly brackets exit
    // You can manually unlock to control the fine granularity of mutexes
    std::unique_lock<std::mutex> locker(g_mutex_lines);

    // Queue is not empty
    if(!g_lines.empty()){
      // Remove the last data from the queue
      line = g_lines.back();

      // Delete the last data in the queue
      g_lines.pop_back();

      // Unlock ahead of time, reduce the fine-grained mutex, and synchronize protection only for shared queue data
      locker.unlock();

      iss = std::istringstream(line);

      do{
        iss >> subs;

        word = "";

        // Remove punctuation
        std::remove_copy_if(subs.begin(), subs.end(),
                      std::back_inserter(word), //Store output
                      std::ptr_fun<int, int>(&std::ispunct)
                     );

        if(word.length() > 0 && (
                      int(word.front()) >= 65 &&
                      int(word.front()) <= 90) || (
                      int(word.front()) >= 97 &&
                      int(word.front()) <= 122)){
         int id;

         id = calc_queue(int(word.front()), workers);

         std::unique_lock<std::mutex> lockerwords(g_mutex_words[id]);
         g_words[id].push_front(word);
         lockerwords.unlock();

         counter += 1;
       }

      } while (iss);

      int counters = 0;

      for(int counter = 0; counter < workers; counter++){
        counters += g_words[counter].size();
      }
      if(counters > 1000){
        consume = true;
      }

      if(consume){
        // Wake up a thread
        g_cond.notify_one();

        while(counters > 200 && producer_is_running){
          std::this_thread::sleep_for(std::chrono::milliseconds(5));

          counters = 0;

          for(int counter = 0; counter < workers; counter++){
            counters += g_words[counter].size();
          }
        }
        consume = false;
      }

      // std::cout << "Parser[" << id << "] : " << data << std::endl;

    }
    // Queue empty
    else
    {
      // The wait() function calls the mutually exclusive unlock() function first, then sleeps itself, and after waking up, it continues to hold the lock, protecting the queue operations that follow.
      // You must use unique_lock, not lock_guard, because lock_guard does not have lock and unlock interfaces, and unique_lock provides both
      // g_cond.wait(locker);

      locker.unlock();
    }

    for(int i = 0; i < workers; i++){
      // std::cout << "ID " << i << ": " << g_words[i].size() << " ";
      counter += g_words[i].size();
    }
    // std::cout << '\n';
  } while(producer_is_running || g_lines.size() > 0);
}

// Parser Thread Function
void Parser(int id, int workers, Spellcheck &spellcheck, Dictionary &dictionary){
    std::string line;
    std::string subs;
    std::string word;
    std::string wordNorm;

    bool found = false;

    std::istringstream iss;

    int counter = 0;


    do{
      // Smart lock, lock when initialized, protect within code curly brackets, and automatically unlock when curly brackets exit
      // You can manually unlock to control the fine granularity of mutexes
      std::unique_lock<std::mutex> locker(g_mutex_words[id]);

      // Queue is not empty
      if(!g_words[id].empty()){
        // Remove the last data from the queue
        word = g_words[id].back();

        // Delete the last data in the queue
        g_words[id].pop_back();

        // Unlock ahead of time, reduce the fine-grained mutex, and synchronize protection only for shared queue data
        locker.unlock();

        found = false;

        // TODO Check why empty word is provided by function
        // Exclude empty words and numbers
        if (word.length() > 0 && isInteger(word) == false){

         std::locale loc;

         // Check if normalized word is in the m_Spellcheck and add it
         if(!spellcheck.find(word)){ // If word with capital letter cannot be found
           if(word.front() != std::tolower(word.front(), loc)){
             wordNorm = word;
             std::string first = Normalize(word.substr(0,1));
             wordNorm.front() = first[0];

             if (spellcheck.find(wordNorm)){ // but can be found with lower case letter, take this one
               word = wordNorm;
               found = true;
             }
           }
         } else {
           found = true;
         }

         if (found){
           dictionary.addWord(word);
         } else {
           spellcheck.missing(word);
         }
       }

        // std::cout << "Parser[" << id << "] : " << data << std::endl;

      }
      // Queue empty
      else
      {
        // The wait() function calls the mutually exclusive unlock() function first, then sleeps itself, and after waking up, it continues to hold the lock, protecting the queue operations that follow.
        // You must use unique_lock, not lock_guard, because lock_guard does not have lock and unlock interfaces, and unique_lock provides both
        // g_cond.wait(locker);

        locker.unlock();
      }

    } while( producer_is_running );

    // std::cout << "Parser[" << id << "] : " << counter << std::endl;
}

int main(int argc, const char *argv[]){

  // Files
  std::ifstream m_InputFile;

  // Variables
  std::string line;
  std::string status = "Lines read: ";
  long count;

  std::vector<std::string> wordvector;

  // Containers
  std::vector<std::string> cmdLineArgs(argv, argv+argc);

  // Process command line arguments
  for(auto& arg : cmdLineArgs){
    if(arg == "--help" || arg == "-help"){
      std::cout << "usage: create-dictionary [options] input_file" << '\n' << std::flush;
      std::cout << "  options:" << '\n' << std::flush;
      std::cout << "    -s        spellcheck_file of same locale" << '\n' << std::flush;
      std::cout << "    -a        android_dictionary_file of same locale" << '\n' << std::flush;
      std::cout << '\n' << std::flush;
      exit(0);
    } else if(arg == "whatever"){

    }
  }

  // Exit if no filename provided
  if (!argv[1]){
    std::cout << "Please provide a correct path and filename." << '\n' << std::flush;
    exit(0);
  }

  // Initiate objects
  Configuration m_Configuration;
  // Spellcheck m_Spellcheck(std::stoi(m_Configuration.value("minimum_missing_spellcheck")));
  Dictionary m_Dictionary;

  // Open the dictionary file to read data
  m_InputFile.open(argv[1]);

  // Exit if filename is wrong
  if(!m_InputFile){
    std::cout << "File not found. Please provide a correct path and filename." << '\n' << std::flush;
    exit(0);
  }

  // Load Android dictionary
  m_Dictionary.loadDict("demodata/de_wordlist.combined");
  m_Dictionary.loadDict("demodata/de_wordlist.custom");

  // Count every line of the file
  for (count = 0; std::getline(m_InputFile, line); ++count){
    if(count % 1000 == 0){
      std::cout << status << count << '\r' << std::flush;
    }
  }

  std::cout << status << count << '\n'  << std::flush;

  // Return to the beginning of the input file
  m_InputFile.clear();
  m_InputFile.seekg(0);

  // Variables for storing data from file
  std::string dictWord;
  std::string dictCount;

  status = "Parsed lines: ";

  auto start = std::chrono::high_resolution_clock::now();

  std::string subs;
  std::string word;

  std::cout << "1 file reader start ..." << std::endl;
  std::thread filereader(FileReader, argv[1]);

  //may return 0 when not able to detect
  // const auto processor_count = std::thread::hardware_concurrency();
  // int consumer_processes = processor_count - 2;
  int consumer_processes = 7;

  std::cout << "2 line parser start ..." << std::endl;
  std::thread linereader(LineReader, consumer_processes);

  std::cout << consumer_processes << " consumer start ..." << std::endl;
  std::thread consumer[consumer_processes];

  Spellcheck m_Spellcheck[consumer_processes];

  for(int i = 0; i < consumer_processes; i++){
    m_Spellcheck[i].set(i, consumer_processes);
    m_Spellcheck[i].open("demodata/German_de_DE.dic");
    consumer[i] = std::thread(Parser, i, consumer_processes, std::ref(m_Spellcheck[i]), std::ref(m_Dictionary));
  }

  filereader.join();
  linereader.join();

  for(int i = 0; i < consumer_processes; i++){
    consumer[i].join();
  }

  std::cout << "All threads joined." << std::endl;

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
  std::cout << "Duration: " << duration << " milliisec \n" << std::flush;

  std::cout << status << "100%" << '\n' << std::flush;

  m_Dictionary.sortCount();
  m_Dictionary.addFrequency();
  m_Dictionary.sortFrequency();

  std::map<std::string, unsigned int> MissingSpellcheck;

  for(int i = 1; i < consumer_processes; i++){
    std::map<std::string, unsigned int> temp = m_Spellcheck[i].getMissingSpellcheck();
    m_Spellcheck[0].append(temp);
  }

  m_Spellcheck[0].exportFile(m_Dictionary);
  m_Dictionary.exportFile();

  // t1 = std::thread(&Dictionary::exportFile, &m_Dictionary);
  // t2 = std::thread(&Spellcheck::exportFile, &m_Spellcheck, m_Dictionary);
  //
  // t1.join();
  // t2.join();

  // Close files
  m_InputFile.close();

  std::cout << "Completed." << '\n' << std::flush;
}
