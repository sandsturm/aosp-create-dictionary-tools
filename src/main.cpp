#include <algorithm>
#include <iostream>
#include <sstream>
#include <thread>
#include <unicode/unistr.h>

#include "configuration.h"
#include "dictionary.h"
#include "spellcheck.h"
#include "threads.h"

static const int num_threads = 4;

inline bool isInteger(const std::string & s){
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

   char * p;
   strtol(s.c_str(), &p, 10);

   return (*p == 0);
}

void print_status(std::string status, std::string buffer){
  // mutex.lock();
  std::cout << status << buffer << "%" << '\r' << std::flush;
  // mutex.unlock();
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

// bool read_sourcefile(std::ifstream m_InputFile, std::vector<std::string> &wordvector){
//   std::string line;
//
//   for (long i = 0; std::getline(m_InputFile, line); ++i){
//     std::istringstream iss(line);
//
//     do{
//       std::string subs;
//       iss >> subs;
//
//       // Remove punctuation
//       std::string word;
//       std::remove_copy_if(subs.begin(), subs.end(),
//                     std::back_inserter(word), //Store output
//                     std::ptr_fun<int, int>(&std::ispunct)
//                    );
//
//       // TODO Check why empty word is provided by function
//       // Exclude empty words and numbers
//       if (word.length() > 0 && isInteger(word) == false){
//         wordvector.push_back(word);
//       }
//     } while (iss);
//   }
//
//   return true;
// }


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

  // Threads
  std::thread t[num_threads];

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
  // m_Spellcheck.open(m_Configuration.value("spellcheck_dictionary"));
  // m_Spellcheck.open(m_Configuration.value("spellcheck_custom"));

  // Load Android dictionary
  std::thread t2(&Dictionary::loadAndroid, &m_Dictionary);

  //Join the threads with the main thread
  t1.join();
  t2.join();
  m_Spellcheck.open(m_Configuration.value("spellcheck_custom"));

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
      std::string subs;
      iss >> subs;

      // Remove punctuation
      std::string word;
      std::remove_copy_if(subs.begin(), subs.end(),
                    std::back_inserter(word), //Store output
                    std::ptr_fun<int, int>(&std::ispunct)
                   );

      // TODO Check why empty word is provided by function
      // Exclude empty words and numbers
      if (word.length() > 0 && isInteger(word) == false){
        bool found = false;

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
  t2 = std::thread(&Spellcheck::exportFile, &m_Spellcheck);

  t1.join();
  t2.join();

  // Close files
  m_InputFile.close();

  std::cout << "Completed." << '\n' << std::flush;
}
