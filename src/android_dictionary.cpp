#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "android_dictionary.h"
#include "dictionary.h"

Android_Dictionary::Android_Dictionary(){
}

Android_Dictionary::~Android_Dictionary(){
}

void Android_Dictionary::open(){
  
}

void Android_Dictionary::add_words(Dictionary *r_Dictionary){
  // Add information from android_dictionary to word vector
  for (std::map<std::string, std::vector<structWord>>::iterator it=dictionary.begin(); it!=dictionary.end(); ++it){
    long row = r_Dictionary->rowWord(it->second[0].word);
    if (row >= 0){
      // std::cout << it->second[0].word << ":" << words[row].word << '\n';
      // Update flags and frequency of existing words
      r_Dictionary->updateWord(row, it->second[0].freq, it->second[0].flags, it->second[0].orgFreq, it->second[0].offensive);
    } else {
      // Add missing words of Android dictionary to the words vector
      r_Dictionary->addWord(it->second[0].word, it->second[0].freq, it->second[0].flags, it->second[0].orgFreq, it->second[0].offensive);
    }
  }
}
