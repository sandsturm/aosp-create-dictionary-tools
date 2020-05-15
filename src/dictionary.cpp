#import "dictionary.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

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

  dictionary.push_back(tempO); // Push structure object into words vector
}

void Dictionary::addWord(std::string s){
  if (findWord(s) != true){
    structWord tempO;

    tempO.word = s;
    tempO.count = 1;

    dictionary.push_back(tempO); // Push structure object into words vector
  }
}

void Dictionary::addWord(std::string s, unsigned int fr, std::string fl, unsigned int org, bool o){
  structWord tempO;

  tempO.word = s;
  tempO.count = 1;
  tempO.freq = fr;
  tempO.flags = fl;
  tempO.orgFreq = org;
  tempO.offensive = o;

  dictionary.push_back(tempO); // Push structure object into words vector
}

void Dictionary::updateWord(long i, unsigned int fr, std::string fl, unsigned int org, bool o){
  dictionary[i].count = 1;
  dictionary[i].freq = fr;
  dictionary[i].flags = fl;
  dictionary[i].orgFreq = org;
  dictionary[i].offensive = o;
}

void Dictionary::addFrequency(){
  // Calculate the divider to ensure results between 50 and 254
  // int divider = int (count / 205) + 1;
  int divider = int(dictionary.size() / 90) + 10;

  // Add frequency and dont touch devider for Dictionary from Google dictionary
  for (long i = 0; i < long(dictionary.size()); ++i){
    int frequency = ((dictionary.size() - i) / divider);
    dictionary[i].freq = frequency;
    dictionary[i].orgFreq = frequency;
  }
}

void Dictionary::exportFile(){
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
  for (long i = 0; i < long(dictionary.size()); ++i){
  // for (long i = 0; i < long(words.size()) || i < 10000; ++i){

    std::string offensive = "";

    if (dictionary[i].offensive){
      offensive = ",possibly_offensive=true";
    }

    // Format: word=der,f=216,flags=,originalFreq=216
    // std::cout << dictionary[i].word << '\n';
    outputFile << " word=" << dictionary[i].word << ",f=" << dictionary[i].freq << ",flags=" << dictionary[i].flags << ",originalFreq=" << dictionary[i].freq << offensive << '\n';
  }

  outputFile.close();
}

void Dictionary::sort(){
  // Sort word vector
  std::sort(dictionary.begin(), dictionary.end(), CompareFrequency);
  std::cout << "Sorted words." << '\n';
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
  for (long int i = 0; i < dictionary.size(); ++i){
    if (dictionary[i].word.compare(s) == 0){   // Increment count of object if found again
      return i;
    }
  }
  return -1;
}
