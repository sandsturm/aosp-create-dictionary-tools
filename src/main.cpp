#include <bits/stdc++.h>
#include <iostream>
#include <chrono>
#include <vector>

#include "spellcheck.h"
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

/*********************************************
bool findSpellcheck(string, vector<Spellcheck>) -
Linear search for word in vector of structures
**********************************************/
bool findSpellcheck(std::string s, std::vector<Spellcheck>& spellcheck){
  // Search through vector
  for (auto& r : spellcheck){
    if (r.word.compare(s) == 0){
      return true;
    }
  }
  return false;
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

void print_status(std::string status, std::string buffer){
  std::cout << status << buffer << "%" << '\n';
}

int main(int argc, const char *argv[]) {
  // Create the containers
  std::set<std::string> spellcheck;
  std::vector<WordCount> words;
  std::map<std::string, WordCount> android_dictionary;

  std::string line;
  long count;

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

  // Read spellcheck file
  std::ifstream spellcheckFile("demodata/German_de_DE.dic");

  // Iterate through spellcheck file
  for (long i = 0; std::getline(spellcheckFile, line); ++i){
    std::istringstream iss(line);
    std::string delimiter = "/";
    do{
      std::string subs;
      iss >> subs;
      // TODO Check token!
      // Check if the / delimiter is in the line.
      // There are some lines without delimiter.
      std::string token = subs.substr(0, subs.find(delimiter));
      if (token.length() > 0){
        subs = token;
      }
      spellcheck.insert(subs);
    } while (iss);
  }

  // Close spellcheckFile since everthing is in the spellcheck vector
  std::cout << "Spellcheck file loaded." << '\n';
  spellcheckFile.close();

  std::string status = "Lines read: ";

  // Count every line of the file
  for (count = 0; std::getline(inputFile, line); ++count){
    std::cout << status << count << '\r';
  }

  std::cout << status << count << '\n';

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

  // Store file length
  long length = count;

  status = "Parsed lines: ";

  int buffer = 0;
  int localbuffer;

  // Create thread pool
  std::vector<std::thread> Workers;

  auto start = std::chrono::high_resolution_clock::now();

  // Iterate through dict file from top to bottom
  for (long i = 0; std::getline(inputFile, line); ++i){
  // for (long i = 0; std::getline(inputFile, line) && i < 10000; ++i){
    count--;

    localbuffer = 100 - (count * 100/length);

    if (localbuffer > buffer){
      Workers.push_back(std::thread(print_status, status, std::to_string(buffer)));
      buffer = localbuffer;
    }

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
        // Check if normalized word is in the spellcheck and add it
        if (spellcheck.find(word) == spellcheck.end()){ // If word with capital letter cannot be found
          if (spellcheck.find(Normalize(word)) != spellcheck.end()){ // but can be found with lower case letter, take this one
            word = Normalize(word);
          }
        }

        if (findWord(word, words) == true){
          continue;
        } else {
          WordCount tempO(word);
          words.push_back(tempO);     // Push structure object into words vector
        }
      }
    } while (iss);

    // Join threads
    // wait for threads to finnish or kill threads
    for (std::thread & th : Workers){
		// If thread Object is Joinable then Join that thread.
		  if (th.joinable())
			   th.join();
    }
  }

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
  std::cout << duration << '\n';

  std::cout << status << "100%" << '\n';

  // Calculate the divider to ensure results between 50 and 254
  // int divider = int (count / 205) + 1;
  int divider = int(words.size() / 205) + 1;

  // Add frequency and dont touch devider for Words from Google dictionary
  for (long i = 0; i < long(words.size()); ++i){
    int frequency = ((words.size() - i) / divider) + 50;
  }

  // Read spellcheck file
  std::ifstream wordtypeFile("demodata/de_wordlist.combined");

  // Iterate through original dictionary file to capture abbreviations and offensive flag
  for (long i = 0; std::getline(wordtypeFile, line); ++i){
    std::cout << "Line: " << i << '\n';

    std::istringstream iss(line);
    std::string delimiter = ",";

    do{
      std::string subs;
      iss >> subs;

      unsigned first = subs.find("word=");
      unsigned last = subs.find(",");

      if (first < subs.length()){

        std::string word = subs.substr(first + 5, last - first + 5);

        first = subs.find("f=");
        last = subs.find(",", first);

        if (first < subs.length()){

          unsigned int freq = std::stoi(subs.substr(first + 2, last- first + 2));
          unsigned int orgFreq = freq;

          first = subs.find("flags=");
          last = subs.find(",", first);

          if (first < subs.length()){
            std::string flags = subs.substr (first + 6, last - first + 6);

            int offensive = subs.find("possibly_offensive=true");

            if ((flags.length() > 0) || (offensive > 0)){
              if (findWord(word, words) == true){
                continue;
              } else {
                WordCount tempO(word, freq, flags, orgFreq, (offensive > 0));
                words.push_back(tempO);
              }
            }
          }
        }
      }
    } while (iss);
  }

  // Close spellcheckFile since everthing is in the spellcheck vector
  std::cout << "Vector size: " << words.size() << '\n';
  std::cout << "Android dictionary file (abbreviations and offensive word) loaded." << '\n';
  wordtypeFile.close();

  // Sort word vector
  sort(words.begin(), words.end(),CompareWordCount);

  // Export vector to restult file
  for (long i = 0; i < long(words.size()) || i < 1000; ++i){

    std::string flag_offensive = "";

    if (words[i].offensive){
      flag_offensive = ",possibly_offensive=true";
    }

    // Format: word=der,f=216,flags=,originalFreq=216
    outputFile << " word=" << words[i].word << ",f=" << words[i].freq << ",flags=" << words[i].flags << ",originalFreq=" << words[i].freq << flag_offensive << '\n';
  }

  std::cout << "Sorted words." << '\n';

  // Close files
  inputFile.close();
  outputFile.close();

  std::cout << "Completed. Wrote output." << '\n';

  return 0;
}
