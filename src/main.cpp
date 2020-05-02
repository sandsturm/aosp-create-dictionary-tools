#include <bits/stdc++.h>
#include <iostream>
#include <chrono>
#include <vector>

#include "wordcount.h"

inline bool isInteger(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

   char * p;
   strtol(s.c_str(), &p, 10);

   return (*p == 0);
}

/*********************************************
bool findWord(string, vector<WordCount>) -
Linear search for word in vector of structures
**********************************************/
bool findWord(std::string s, std::vector<WordCount>& words){
  // Search through vector
  for (auto& r : words){
    if (r.word.compare(s) == 0){   // Increment count of object if found again
      r.iCount();
      return true;
    }
  }
  return false;
}

int main(int argc, const char *argv[]) {
  // Exit if no filename provided
  if (!argv[1]){
    std::cout << "Please provide a correct path and filename." << '\n';
    exit(0);
  }


  // Open the dictionary file to read data
  std::ifstream inputFile(argv[1]);

  // Exit if filename is wrong
  if (!inputFile){
    std::cout << "File not found. Please provide a correct path and filename." << '\n';
    exit(0);
  }

  std::string line;
  long count;

  // Count every line of the file
  for (count = 0; std::getline(inputFile, line); ++count)
  ;

  // Calculate the divider to ensure results between 50 and 254
  // int divider = int (count / 205) + 1;
  int divider = int(count / 205) + 1;

  // Return to the beginning of the input file
  inputFile.clear();
  inputFile.seekg(0);

  // Open the new dictionary file to write data
  std::ofstream outputFile("demodata/output.txt");

  // Write version and timestamp
  // Format: dictionary=main:de,locale=de,description=Deutsch,date=1414726263,version=54,REQUIRES_GERMAN_UMLAUT_PROCESSING=1
  std::string strLocale = "de";
  std::string strDescription = "Deutsch";
  std::string strVersion = "54";

  outputFile << "dictionary=main:" << strLocale
             << ",locale=" << strLocale
             << ",description=" << strDescription
             << ",date=" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
             << ",version=" << strVersion
             << ",REQUIRES_GERMAN_UMLAUT_PROCESSING=1"
             << '\n';

  // Variables for storing data from file
  std::string dictWord;
  std::string dictCount;

  // Vector of words
  std::vector<WordCount> words;

  // Iterate through dict file from top to bottom
  for (long i = 0; std::getline(inputFile, line); ++i){
    count--;

    std::istringstream iss(line);
    do{
        std::string subs;
        iss >> subs;
        if (findWord(subs, words) == true){
          continue;
        } else {
          WordCount tempO(subs);     // Make structure object with n
          words.push_back(tempO);     // Push structure object into words vector
        }
        // std::cout << "Substring: " << subs << std::endl;
    } while (iss);
  }

  // Sort word vector
  sort(words.begin(), words.end(),CompareWordCount);

  // Export vector to restult file
  for (long i = 0; i < words.size(); ++i){
    outputFile << words[i].word << ":" << words[i].count << '\n';
  }

  // Close files
  inputFile.close();
  outputFile.close();

  return 0;
}
