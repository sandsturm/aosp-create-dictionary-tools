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
#include "functions.h"
#include "spellcheck.h"

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

// Create a callback function
int callback(void *NotUsed, int argc, char **argv, char **azColName){
    // Return successful
    return 0;
}

// Global Queues
std::deque<std::string> g_lines;
std::deque<std::string> g_words;

// Global Lock
std::mutex g_mutex_lines, g_mutex_words, g_mutex_lines_spell, g_mutex_lines_dict;

// Global Conditional Variables
std::condition_variable g_cond, g_spell, g_dict;

// FileReader Run Marker
bool producer_is_running = true;

// FileReader Thread Function
void FileReader(std::string filename){
  int counter = 0;
  std::string line;
  std::ifstream m_InputFile;

  m_InputFile.open(filename);

  while(std::getline(m_InputFile, line)){
    counter += 1;

    if(counter % 900 == 0){
      std::cout << counter << '\r' << std::flush;
    }

    // Smart lock, lock when initialized, protect within code curly brackets, and automatically unlock when curly brackets exit
    // You can manually unlock to control the fine granularity of mutexes
    std::unique_lock<std::mutex> locker(g_mutex_lines);

    // Queue a data
    g_lines.push_front(line);

    // Unlock ahead of time, reduce the fine-grained mutex, and synchronize protection only for shared queue data
    locker.unlock();

    if(g_lines.size() > 130){
      // Wake up a thread
      g_cond.notify_one();
      while(g_lines.size() > 50){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
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

        if((word.length() > 0) && (
                      ((int(word.front()) >= 65) &&
                      (int(word.front()) <= 90))) || (
                      (int(word.front()) >= 97) &&
                      (int(word.front()) <= 122))){

         std::unique_lock<std::mutex> lockerwords(g_mutex_words);
         g_words.push_front(word);
         lockerwords.unlock();
       }

      } while (iss);

      if(g_words.size() > 250){
        // Wake up a thread
        g_cond.notify_one();

        while(g_words.size() > 75 && producer_is_running){
          std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
      }
    }
    // Queue empty
    else
    {
      // The wait() function calls the mutually exclusive unlock() function first, then sleeps itself, and after waking up, it continues to hold the lock, protecting the queue operations that follow.
      // You must use unique_lock, not lock_guard, because lock_guard does not have lock and unlock interfaces, and unique_lock provides both
      // g_cond.wait(locker);

      locker.unlock();
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
    Spellcheck m_Spellcheck;
    Dictionary m_Dictionary;
    std::deque<std::string> words;

    bool found = false;

    std::istringstream iss;

    m_Spellcheck.set(id, workers);
    m_Spellcheck.open("demodata/German_de_DE.dic");

    m_Dictionary.set(id, workers);
    // Load Android dictionary
    m_Dictionary.loadDict("demodata/de_wordlist.combined");
    m_Dictionary.loadDict("demodata/de_wordlist.custom", true);

    do{
      // Smart lock, lock when initialized, protect within code curly brackets, and automatically unlock when curly brackets exit
      // You can manually unlock to control the fine granularity of mutexes
      std::unique_lock<std::mutex> locker(g_mutex_words);

      while (!g_words.empty() && ((calculate_queue(int(g_words.back().front()), workers) == id) || (calculate_queue(int(g_words.front().front()), workers) == id))) {
        if(calculate_queue(int(g_words.back().front()), workers) == id){
          // Remove the last data from the queue
          words.push_front(g_words.back());
          // Delete the last data in the queue
          g_words.pop_back();
        } else {
          // Same with front
          words.push_front(g_words.front());
          g_words.pop_front();
        }
      }

      // Unlock ahead of time, reduce the fine-grained mutex, and synchronize protection only for shared queue data
      locker.unlock();

      // Queue is not empty
      while(!words.empty()){
        // Remove the last data from the queue
        word = words.back();
        // Delete the last data in the queue
        words.pop_back();

        found = false;

        // TODO Check why empty word is provided by function
        // Exclude empty words and numbers
        if (word.length() > 0 && isInteger(word) == false){

         std::locale loc;

         // Check if normalized word is in the m_Spellcheck and add it
         if(!m_Spellcheck.find(word)){ // If word with capital letter cannot be found
           if(word.front() != std::tolower(word.front(), loc)){
             wordNorm = word;
             std::string first = Normalize(word.substr(0,1));
             wordNorm.front() = first[0];

             if (m_Spellcheck.find(wordNorm)){ // but can be found with lower case letter, take this one
               word = wordNorm;
               found = true;
             }
           }
         } else {
           found = true;
         }

         if (found){
           m_Dictionary.addWord(word);
         } else {
           m_Spellcheck.missing(word);
         }
       }
      }

    } while( producer_is_running || !g_words.empty());

    std::unique_lock<std::mutex> lockerspell(g_mutex_lines_spell);
    spellcheck.append(m_Spellcheck.getMissingSpellcheck());
    lockerspell.unlock();

    std::unique_lock<std::mutex> lockerdict(g_mutex_lines_dict);
    dictionary.append(m_Dictionary.getDictionaryEntries());
    lockerdict.unlock();
}

int main(int argc, const char *argv[]){

  // Files
  std::ifstream m_InputFile;

  // Variables
  std::string line;
  std::string status = "Lines read: ";
  long count;

  // Containers
  std::vector<std::string> wordvector;
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
  const auto processor_count = std::thread::hardware_concurrency();
  int consumer_processes = processor_count - 2;
  // int consumer_processes = 7;

  std::cout << "1 line parser start ..." << std::endl;
  std::thread linereader(LineReader, consumer_processes);

  std::cout << consumer_processes << " consumer start ..." << std::endl;
  std::thread consumer[consumer_processes];

  Spellcheck m_Spellcheck;

  for(int i = 0; i < consumer_processes; ++i){
    consumer[i] = std::thread(Parser, i, consumer_processes, std::ref(m_Spellcheck), std::ref(m_Dictionary));
  }

  filereader.join();
  linereader.join();

  for(int i = 0; i < consumer_processes; ++i){
    consumer[i].join();
  }

  std::cout << "All threads joined." << std::endl;

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
  std::cout << "Duration: " << duration << " millisec \n" << std::flush;

  std::cout << status << "100%" << '\n' << std::flush;

  m_Dictionary.sortCount();
  m_Dictionary.addFrequency();
  m_Dictionary.sortFrequency();

  m_Spellcheck.exportFile(m_Dictionary);
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
