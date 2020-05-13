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

long rowWord(std::string s, std::vector<WordCount>& words){
  // Search through vector
  for (long int i = 0; i < words.size(); ++i){
    if (words[i].word.compare(s) == 0){   // Increment count of object if found again
      return i;
    }
  }
  return -1;
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
  std::cout << status << buffer << "%" << '\r' << std::flush;
}

void load_spellcheckfile(std::set<std::string> &spellcheck){
  std::string line;

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
}

int main(int argc, const char *argv[]) {
  // Create the containers
  std::set<std::string> spellcheck;
  std::set<std::string> missingSpellcheck;
  std::vector<WordCount> words;
  std::map<std::string, std::vector<WordCount>> android_dictionary;

  std::string line;
  long count;

  // Create thread pool
  std::vector<std::thread> Workers;

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

  Workers.push_back(std::thread(load_spellcheckfile, std::ref(spellcheck)));

  std::string status = "Lines read: ";

  // Count every line of the file
  for (count = 0; std::getline(inputFile, line); ++count){
    std::cout << status << count << '\r' << std::flush;
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

  for (std::thread & th : Workers){
  // If thread Object is Joinable then Join that thread.
    if (th.joinable())
       th.join();
  }

  status = "Parsed lines: ";

  int buffer = 0;
  int localbuffer;

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
        bool found = false;

        // Check if normalized word is in the spellcheck and add it
        if (spellcheck.find(word) == spellcheck.end()){ // If word with capital letter cannot be found
          if (spellcheck.find(Normalize(word)) != spellcheck.end()){ // but can be found with lower case letter, take this one
            word = Normalize(word);
            found = true;
          }
        } else {
          found = true;
        }

        if (found){
          if (findWord(word, words) == true){
            continue;
          } else {
            WordCount tempO(word);
            words.push_back(tempO);     // Push structure object into words vector
          }
        } else {
          missingSpellcheck.insert(word);
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

  std::cout << status << "100%" << '\n';

  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
  std::cout << "Duration: " << duration << '\n';

  // Calculate the divider to ensure results between 50 and 254
  // int divider = int (count / 205) + 1;
  int divider = int(words.size() / 90) + 10;

  // Add frequency and dont touch devider for Words from Google dictionary
  for (long i = 0; i < long(words.size()); ++i){
    int frequency = ((words.size() - i) / divider);
    words[i].freq = frequency;
    words[i].orgFreq = frequency;
  }

  // Read spellcheck file
  std::ifstream wordtypeFile("demodata/de_wordlist.combined");

  // Iterate through Android dictionary file to capture abbreviations and offensive flag
  for (long i = 0; std::getline(wordtypeFile, line); ++i){
    std::istringstream iss(line);
    std::string delimiter = ",";

    do{
      std::string subs;
      iss >> subs;

      unsigned first = subs.find("word=");
      unsigned last = subs.find(",");

      if (first < subs.length()){

        std::string word = subs.substr(first + 5, last - first - 5);

        first = subs.find("f=");
        last = subs.find(",", first);

        if (first < subs.length()){

          unsigned int freq = std::stoi(subs.substr(first + 2, last - first - 2));
          first = subs.find("originalFreq=");
          last = subs.find(",", first);

          if (first < subs.length()){

            unsigned int originalFreq = std::stoi(subs.substr(first + 13, last - first - 13));

            first = subs.find("flags=");
            last = subs.find(",", first);

            if (first < subs.length()){
              std::string flags = subs.substr (first + 6, last - first - 6);

              bool offensive = false;

              if (subs.find("possibly_offensive=true") < subs.length()){
                offensive = true;
              }

              WordCount tempO(word, freq, flags, originalFreq, offensive);
              android_dictionary[word];
              android_dictionary[word].push_back(tempO);
            }
          }
        }
      }
    } while (iss);
  }

  // Close Android dictionary file
  std::cout << "Vector size: " << android_dictionary.size() << '\n';
  std::cout << "Android dictionary file (abbreviations and offensive word) loaded." << '\n';
  wordtypeFile.close();

  // Add information from android_dictionary to word vector
  for (std::map<std::string, std::vector<WordCount>>::iterator it=android_dictionary.begin(); it!=android_dictionary.end(); ++it){
    long row = rowWord(it->second[0].word, words);
    if (row >= 0){
      // std::cout << it->second[0].word << ":" << words[row].word << '\n';
      // Update flags and frequency of existing words
      words[row].freq = it->second[0].freq;
      words[row].flags = it->second[0].flags;
      words[row].orgFreq = it->second[0].orgFreq;
      words[row].offensive = it->second[0].offensive;
    } else {
      // Add missing words of Android dictionary to the words vector
      WordCount tempO(it->second[0].word, it->second[0].freq, it->second[0].flags, it->second[0].orgFreq, it->second[0].offensive);
      words.push_back(tempO);
    }
  }

  std::cout << "Android dictionary file aligned with dictionary." << '\n';

  // Sort word vector
  sort(words.begin(), words.end(), CompareFrequency);
  std::cout << "Sorted words." << '\n';

  // Export vector to restult file
  for (long i = 0; i < long(words.size()); ++i){
  // for (long i = 0; i < long(words.size()) || i < 10000; ++i){

    std::string offensive = "";

    if (words[i].offensive){
      offensive = ",possibly_offensive=true";
    }

    // Format: word=der,f=216,flags=,originalFreq=216
    // std::cout << words[i].word << '\n';
    outputFile << " word=" << words[i].word << ",f=" << words[i].freq << ",flags=" << words[i].flags << ",originalFreq=" << words[i].freq << offensive << '\n';
  }

  // Close files
  inputFile.close();
  outputFile.close();

  // Export missing spellcheck
  // Open file to write missing spellcheck
  std::ofstream missesFile("demodata/missing_spellcheck.txt");
  for(auto f : missingSpellcheck) {
    missesFile << f << '\n';
  }

  missesFile.close();

  std::cout << "Completed. Wrote output." << '\n';

  return 0;
}
