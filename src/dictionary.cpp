#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "dictionary.h"
#include "functions.h"

Dictionary::Dictionary(){
}

Dictionary::~Dictionary(){
}

Dictionary::Dictionary(std::string s){
  structWord tempO;

  tempO.word = s;
  tempO.count = 1;
  tempO.flags = "";
  tempO.offensive = false;

  dictionary.push_back(tempO); // Push structure object into words vector
}

Dictionary::Dictionary(std::string s, unsigned int fr, std::string fl, unsigned int org, bool o){
  structWord tempO;

  tempO.word = s;
  tempO.count = 1;
  tempO.freq = fr;
  tempO.flags = fl;
  tempO.orgFreq = org;
  tempO.offensive = o;
  tempO.android = 0;

  dictionary.push_back(tempO); // Push structure object into words vector
}

void Dictionary::set(int id, int workers){
  m_Id = id;
  m_Workers = workers;
}

void Dictionary::addWord(std::string s){
  if (findWord(s) != true){
    structWord tempO;

    tempO.word = s;
    tempO.count = 1;
    tempO.android = 0;

    dictionary.push_back(tempO); // Push structure object into words vector
  }
}

void Dictionary::addFrequency(){
  // Calculate the divider to ensure results between 50 and 254
  // int divider = int (count / 205) + 1;
  int divider = int(dictionary.size() / DIVIDER) + BASEFREQ;

  // Add frequency and dont touch devider for Dictionary from Google dictionary
  for (long i = 0; i < long(dictionary.size()); ++i){
    if (dictionary[i].android == 0){
      int frequency = ((dictionary.size() - i) / divider);
      dictionary[i].freq = frequency;
      dictionary[i].orgFreq = frequency;
    }
  }
}

void Dictionary::loadDict(std::string fileName, bool custom){
  // Read Android dictionary file
  std::ifstream file(fileName);
  std::string delimiter = ",";
  std::string line;

  // Iterate through Android dictionary file to capture abbreviations and offensive flag
  for (long i = 0; std::getline(file, line); ++i){
    std::istringstream iss(line);

    // Make sure the custom dictionary has the same structure
    // word=Unterforen,f=20,flags=,originalFreq=20
    // f=20 is just a very low frequency numnber, random
    if(custom){
      std::string s;
      s.append("word=");
      s.append(line);
      s.append(",f=0,flags=,originalFreq=0");
      iss.str(s);
    }

    do{
      std::string subs;
      iss >> subs;

      unsigned first = subs.find("word=");
      unsigned last = subs.find(",");

      if (first < subs.length()){

        std::string word = subs.substr(first + 5, last - first - 5);

        if(calculate_queue(word.front(), m_Workers) == m_Id){
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

                structWord tempO;

                tempO.word = word;
                tempO.count = 1;
                tempO.freq = freq;
                tempO.flags = flags;
                tempO.orgFreq = originalFreq;
                tempO.offensive = offensive;

                if(custom){
                  tempO.android = 0;
                } else {
                  tempO.android = 1;
                }

                dictionary.push_back(tempO);
              }
            }
          }
        }
      }
    } while (iss);
  }

  // Close Android dictionary file
  file.close();
}

void Dictionary::sortCount(){
  // Sort word vector
  std::sort(dictionary.begin(), dictionary.end(), CompareWordcount);
  std::cout << "Sorted words by count." << '\n';
}

void Dictionary::sortFrequency(){
  // Sort word vector
  std::sort(dictionary.begin(), dictionary.end(), CompareFrequency);
  std::cout << "Sorted words by frequency." << '\n';
}

/*********************************************
bool findWord(string, vector<Dictionary>) -
Linear search for word in vector of structures
**********************************************/
bool Dictionary::findWord(std::string s){
  // Search through vector
  for (auto& r : dictionary){
    if (r.word.compare(s) == 0){   // Increment count of object if found again
      r.iCount();
      return true;
    }
  }
  return false;
}

long Dictionary::rowWord(std::string s){
  // Search through vector
  for (long unsigned int i = 0; i < dictionary.size(); ++i){
    if (dictionary[i].word.compare(s) == 0){
      return i;
    }
  }
  return -1;
}

void Dictionary::append(std::vector<structWord> externdict){
  dictionary.insert(dictionary.end(), externdict.begin(), externdict.end());
}

std::vector<structWord> Dictionary::getDictionaryEntries(){
  return dictionary;
}

void Dictionary::exportFile(){
  // Open the new dictionary file to write data
  std::ofstream outputFile("demodata/output.txt");

  // Write version and timestamp
  // Format: dictionary=main:de,locale=de,description=Deutsch,date=1414726263,version=54,REQUIRES_GERMAN_UMLAUT_PROCESSING=1
  std::string strLocale = "de";
  std::string strDescription = "Deutsch";
  std::string strVersion = "56";

  outputFile << "dictionary=main:" << strLocale
             << ",locale=" << strLocale
             << ",description=" << strDescription
             << ",date=" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
             << ",version=" << strVersion
             << ",REQUIRES_GERMAN_UMLAUT_PROCESSING=1"
             << '\n';

  int counter = 0;

  // Export vector to result file
  for (long i = 0; i < long(dictionary.size()); ++i){
  // for (long i = 0; i < long(words.size()) || i < 10000; ++i){

    std::string offensive = "";

    if (dictionary[i].offensive){
      offensive = ",possibly_offensive=true";
    }

    // Format: word=der,f=216,flags=,originalFreq=216
    // std::cout << dictionary[i].word << '\n';
    outputFile << " word=" << dictionary[i].word << ",f=" << dictionary[i].freq << ",flags=" << dictionary[i].flags << ",originalFreq=" << dictionary[i].freq << offensive << '\n';
    counter += 1;

  }

  std::cout << "Dictionary file exported, " << counter << " entries." << '\n';

  outputFile.close();
}
