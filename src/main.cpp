#include <algorithm>
#include <iostream>
#include <sstream>

#include "android_dictionary.h"
#include "spellcheck.h"
#include "dictionary.h"
#include "threads.h"

inline bool isInteger(const std::string & s)
{
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
    char c;

    // Make all text lowercase
    for (int i = 0; i < int(s.length()); i++){
        c = s[i];
        c = tolower(c);
        if (isalpha(c) || isblank(c))
            nString += c;
    }

    // Return converted string
    return nString;
}


int main(int argc, const char *argv[]) {
  // Files
  std::ifstream m_InputFile;

  // Variables
  std::string line;
  std::string status = "Lines read: ";
  long count;

  // Initiate objects
  Android_Dictionary m_AndroidDictionary;
  Spellcheck m_Spellcheck;
  Dictionary m_Dictionary;

  // Exit if no filename provided
  if (!argv[1]){
    std::cout << "Please provide a correct path and filename." << '\n' << std::flush;
    exit(0);
  }

  // Open the dictionary file to read data
  m_InputFile.open(argv[1]);

  // Exit if filename is wrong
  if (!m_InputFile){
    std::cout << "File not found. Please provide a correct path and filename." << '\n' << std::flush;
    exit(0);
  }

  m_Spellcheck.open();
  m_Dictionary.loadAndroid();

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
  // for (long i = 0; std::getline(m_InputFile, line); ++i){
  for (long i = 0; std::getline(m_InputFile, line) && i < 10000; ++i){
    count--;

    localbuffer = 100 - (count * 100/length);

    if (localbuffer > buffer){
      // Workers.push_back(std::thread(print_status, status, std::to_string(buffer)));
      buffer = localbuffer;
    }

    std::cout << status << buffer << "%" << '\r' << std::flush;

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
        if (m_Spellcheck.find(word)){ // If word with capital letter cannot be found
          if (!m_Spellcheck.find(Normalize(word))){ // but can be found with lower case letter, take this one
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
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
  std::cout << "Duration: " << duration << '\n' << std::flush;

  m_Dictionary.sort();
  m_Dictionary.exportFile();
  m_Spellcheck.exportFile();

  // Close files
  m_InputFile.close();

  std::cout << "Wrote output." << '\n' << std::flush;
  std::cout << "Completed." << '\n' << std::flush;
}
