#ifndef STRUCT_WORDS_H
#define STRUCT_WORDS_H
#pragma once

#include <string>

struct structWordcount {
  std::string word;    // Word
  unsigned int count;      // Occurence #
  unsigned int freq;
  unsigned int orgFreq;
  std::string flags;
  bool offensive;

  void iCount(){
    count++;
  }
};


struct {
  bool operator()(const structWordcount& a, const structWordcount& b){
    if (a.count < b.count)
        return false;
    else if (a.count > b.count)
        return true;
    else{
        if (a.word < b.word)
            return true;
        else
            return false;
    }
  }
} CompareWordcount;

struct {
  bool operator()(const structWordcount& a, const structWordcount& b){
    if (a.freq < b.freq)
        return false;
    else if (a.freq > b.freq)
        return true;
    else{
        if (a.word < b.word)
            return true;
        else
            return false;
    }
  }
} CompareFrequency;

#endif // STRUCT_WORDS_H ///:~
