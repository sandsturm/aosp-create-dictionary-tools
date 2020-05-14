#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "android_dictionary.h"
#include "wordcount.h"

Android_Dictionary::Android_Dictionary(){
}

Android_Dictionary::~Android_Dictionary(){
}

void Android_Dictionary::open(){
  // Read Android dictionary file
  std::ifstream file("demodata/de_wordlist.combined");
  std::string line;

  // Iterate through Android dictionary file to capture abbreviations and offensive flag
  for (long i = 0; std::getline(file, line); ++i){
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

              structWordcount tempO;

              tempO.word = word;
              tempO.count = 1;
              tempO.freq = freq;
              tempO.flags = flags;
              tempO.orgFreq = originalFreq;
              tempO.offensive = offensive;

              dictionary[word].push_back(tempO);
            }
          }
        }
      }
    } while (iss);
  }

  // Close Android dictionary file
  std::cout << "Vector size: " << dictionary.size() << '\n';
  std::cout << "Android dictionary file (abbreviations and offensive word) loaded." << '\n';
  file.close();
}

void Android_Dictionary::add_words(Wordcount *r_Wordcount){
  // Add information from android_dictionary to word vector
  for (std::map<std::string, std::vector<structWordcount>>::iterator it=dictionary.begin(); it!=dictionary.end(); ++it){
    long row = r_Wordcount->rowWord(it->second[0].word);
    if (row >= 0){
      // std::cout << it->second[0].word << ":" << words[row].word << '\n';
      // Update flags and frequency of existing words
      r_Wordcount->updateWord(row, it->second[0].freq, it->second[0].flags, it->second[0].orgFreq, it->second[0].offensive);
    } else {
      // Add missing words of Android dictionary to the words vector
      r_Wordcount->addWordcount(it->second[0].word, it->second[0].freq, it->second[0].flags, it->second[0].orgFreq, it->second[0].offensive);
    }
  }
}
