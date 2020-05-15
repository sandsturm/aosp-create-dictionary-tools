#import "wordcount.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

Wordcount::Wordcount(){
}

Wordcount::~Wordcount(){
}

Wordcount::Wordcount(std::string s){
  structWordcount tempO;

  tempO.word = s;
  tempO.count = 1;
  tempO.flags = "";
  tempO.offensive = false;

  wordcount.push_back(tempO); // Push structure object into words vector
}

Wordcount::Wordcount(std::string s, unsigned int fr, std::string fl, unsigned int org, bool o){
  structWordcount tempO;

  tempO.word = s;
  tempO.count = 1;
  tempO.freq = fr;
  tempO.flags = fl;
  tempO.orgFreq = org;
  tempO.offensive = o;

  wordcount.push_back(tempO); // Push structure object into words vector
}

void Wordcount::addWord(std::string s){
  if (findWord(s) != true){
    structWordcount tempO;

    tempO.word = s;
    tempO.count = 1;

    wordcount.push_back(tempO); // Push structure object into words vector
  }
}

void Wordcount::addWordcount(std::string s, unsigned int fr, std::string fl, unsigned int org, bool o){
  structWordcount tempO;

  tempO.word = s;
  tempO.count = 1;
  tempO.freq = fr;
  tempO.flags = fl;
  tempO.orgFreq = org;
  tempO.offensive = o;

  wordcount.push_back(tempO); // Push structure object into words vector
}

void Wordcount::updateWord(long i, unsigned int fr, std::string fl, unsigned int org, bool o){
  wordcount[i].count = 1;
  wordcount[i].freq = fr;
  wordcount[i].flags = fl;
  wordcount[i].orgFreq = org;
  wordcount[i].offensive = o;
}

void Wordcount::addFrequency(){
  // Calculate the divider to ensure results between 50 and 254
  // int divider = int (count / 205) + 1;
  int divider = int(wordcount.size() / 90) + 10;

  // Add frequency and dont touch devider for Wordcount from Google dictionary
  for (long i = 0; i < long(wordcount.size()); ++i){
    int frequency = ((wordcount.size() - i) / divider);
    wordcount[i].freq = frequency;
    wordcount[i].orgFreq = frequency;
  }
}

void Wordcount::exportFile(){
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

  // Export vector to result file
  for (long i = 0; i < long(wordcount.size()); ++i){
  // for (long i = 0; i < long(words.size()) || i < 10000; ++i){

    std::string offensive = "";

    if (wordcount[i].offensive){
      offensive = ",possibly_offensive=true";
    }

    // Format: word=der,f=216,flags=,originalFreq=216
    // std::cout << wordcount[i].word << '\n';
    outputFile << " word=" << wordcount[i].word << ",f=" << wordcount[i].freq << ",flags=" << wordcount[i].flags << ",originalFreq=" << wordcount[i].freq << offensive << '\n';
  }

  outputFile.close();
}

void Wordcount::sort(){
  // Sort word vector
  std::sort(wordcount.begin(), wordcount.end(), CompareFrequency);
  std::cout << "Sorted words." << '\n';
}

/*********************************************
bool findWord(string, vector<Wordcount>) -
Linear search for word in vector of structures
**********************************************/
bool Wordcount::findWord(std::string s){
  // Search through vector
  for (auto& r : wordcount){
    if (r.word.compare(s) == 0){   // Increment count of object if found again
      r.iCount();
      return true;
    }
  }
  return false;
}

long Wordcount::rowWord(std::string s){
  // Search through vector
  for (long int i = 0; i < wordcount.size(); ++i){
    if (wordcount[i].word.compare(s) == 0){   // Increment count of object if found again
      return i;
    }
  }
  return -1;
}
