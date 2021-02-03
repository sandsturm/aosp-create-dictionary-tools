#include <algorithm>
#include <iostream>
#include <sstream>
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

// Global Queue
std::deque<std::string> g_deque;

// Global Lock
std::mutex g_mutex;

// Global Conditional Variables
std::condition_variable g_cond;

// Producer Run Marker
bool producer_is_running = true;

// Producer Thread Function
void Producer(std::string filename){
  // Number of Inventories
  std::string line;
  std::ifstream m_InputFile;

  bool consume = false;

  m_InputFile.open(filename);

  while(std::getline(m_InputFile, line)){
    // Smart lock, lock when initialized, protect within code curly brackets, and automatically unlock when curly brackets exit
    // You can manually unlock to control the fine granularity of mutexes
    std::unique_lock<std::mutex> locker( g_mutex );

    // Queue a data
    g_deque.push_back(line);

    if(g_deque.size() > 100){
      consume = true;
    }

    if(consume){
      // Unlock ahead of time, reduce the fine-grained mutex, and synchronize protection only for shared queue data
      locker.unlock();

      // Wake up a thread
      g_cond.notify_all();

      while(g_deque.size() > 10){
        std::cout << "Deque size: " << g_deque.size() << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }

      consume = false;
    }
  }

  m_InputFile.close();

  // Mark Producer Proofed
  producer_is_running = false;

  // Wake up all consumer threads
  g_cond.notify_all();

  std::cout << "Producer    : I'm out of stock. I'm proofing!"  << std::endl;
}

// Consumer Thread Function
void Consumer(int id){
    // Item number purchased
    std::string data;

    do{
      // Smart lock, lock when initialized, protect within code curly brackets, and automatically unlock when curly brackets exit
      // You can manually unlock to control the fine granularity of mutexes
      std::unique_lock<std::mutex> locker( g_mutex );

      // The wait() function calls the mutually exclusive unlock() function first, then sleeps itself, and after waking up, it continues to hold the lock, protecting the queue operations that follow.
      // You must use unique_lock, not lock_guard, because lock_guard does not have lock and unlock interfaces, and unique_lock provides both
      g_cond.wait(locker);

      // Queue is not empty
      if(!g_deque.empty()){
        // Remove the last data from the queue
        data = g_deque.back();

        // Delete the last data in the queue
        g_deque.pop_back();

        // Unlock ahead of time, reduce the fine-grained mutex, and synchronize protection only for shared queue data
        locker.unlock();
        g_cond.notify_all();

        std::cout << "Consumer[" << id << "] : " << data << std::endl;
      }
      // Queue empty
      else
      {
        locker.unlock();
      }

    } while( producer_is_running );

    std::cout << "Consumer[" << id << "] : The seller has no proofing. What a pity, come back next time!"  << std::endl;
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
  Spellcheck m_Spellcheck(std::stoi(m_Configuration.value("minimum_missing_spellcheck")));
  Dictionary m_Dictionary;

  // Open the dictionary file to read data
  m_InputFile.open(argv[1]);

  // Exit if filename is wrong
  if(!m_InputFile){
    std::cout << "File not found. Please provide a correct path and filename." << '\n' << std::flush;
    exit(0);
  }

  // Load spellcheck files
  std::thread t1(&Spellcheck::open, &m_Spellcheck, m_Configuration.value("spellcheck_dictionary"));

  // Load Android dictionary
  std::thread t2(&Dictionary::loadDict, &m_Dictionary, "demodata/de_wordlist.combined");

  // m_Spellcheck.open(m_Configuration.value("spellcheck_dictionary"));
  // m_Spellcheck.open(m_Configuration.value("spellcheck_custom"));

  //Join the threads with the main thread
  t1.join();
  t2.join();

  t1 = std::thread(&Spellcheck::open, &m_Spellcheck, m_Configuration.value("spellcheck_custom"));
  t2 = std::thread(&Dictionary::loadDict, &m_Dictionary, "demodata/de_wordlist.custom");

  //Join the threads with the main thread
  t1.join();
  t2.join();

  // Count every line of the file
  for (count = 0; std::getline(m_InputFile, line); ++count){
    std::cout << status << count << '\r' << std::flush;
  }

  std::cout << status << count << '\n'  << std::flush;

  // Return to the beginning of the input file
  m_InputFile.clear();
  m_InputFile.seekg(0);

  // Variables for storing data from file
  std::string dictWord;
  std::string dictCount;

  // Store file length
  long length = count;

  // for (std::thread & th : Workers){
  // // If thread Object is Joinable then Join that thread.
  //   if (th.joinable())
  //      th.join();
  // }

  status = "Parsed lines: ";

  int buffer = 0;
  int localbuffer;

  auto start = std::chrono::high_resolution_clock::now();

  std::string subs;
  std::string word;
  bool found;

  std::cout << "1 producer start ..." << std::endl;
  std::thread producer(Producer, argv[1]);

  std::cout << "5 consumer start ..." << std::endl;
  std::thread consumer[ 5 ];
  for(int i = 0; i < 5; i++){
      consumer[i] = std::thread(Consumer, i + 1);
  }

  producer.join();

  for(int i = 0; i < 5; i++){
      consumer[i].join();
  }

  std::cout << "All threads joined." << std::endl;
  exit(0);

  // Iterate through dict file from top to bottom
  for (long i = 0; std::getline(m_InputFile, line); ++i){
  // for (long i = 0; std::getline(m_InputFile, line) && i < 10000; ++i){
    count--;

    localbuffer = 100 - (count * 100 / length);

    if (localbuffer > buffer){
      // Workers.push_back(std::thread(print_status, status, std::to_string(buffer)));
      buffer = localbuffer;
    }

    std::cout << status << buffer << "% / " << count << '\r' << std::flush;

    std::istringstream iss(line);

    do{
      iss >> subs;

      // Remove punctuation
      std::remove_copy_if(subs.begin(), subs.end(),
                    std::back_inserter(word), //Store output
                    std::ptr_fun<int, int>(&std::ispunct)
                   );

      // TODO Check why empty word is provided by function
      // Exclude empty words and numbers
      if (word.length() > 0 && isInteger(word) == false){
        found = false;

        // Check if normalized word is in the m_Spellcheck and add it
        if (!m_Spellcheck.find(word)){ // If word with capital letter cannot be found
          if (m_Spellcheck.find(Normalize(word))){ // but can be found with lower case letter, take this one
            word = Normalize(word);
            found = true;
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
    } while (iss);

    // Join threads
    // wait for threads to finnish or kill threads
    // for (std::thread & th : Workers){
    //     // If thread Object is Joinable then Join that thread.
    //       if (th.joinable())
    //            th.join();
    // }
  }

  std::cout << status << "100%" << '\n' << std::flush;

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
  std::cout << "Duration: " << duration << " milliisec \n" << std::flush;

  m_Dictionary.sortCount();
  m_Dictionary.addFrequency();
  m_Dictionary.sortFrequency();

  t1 = std::thread(&Dictionary::exportFile, &m_Dictionary);
  t2 = std::thread(&Spellcheck::exportFile, &m_Spellcheck, m_Dictionary);

  t1.join();
  t2.join();

  // Close files
  m_InputFile.close();

  std::cout << "Completed." << '\n' << std::flush;
}
