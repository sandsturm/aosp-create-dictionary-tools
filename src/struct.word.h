#ifndef STRUCT_WORD_H
#define STRUCT_WORD_H
#pragma once

#include <string>

struct structWord {
  std::string word;    // Word
  unsigned int count;      // Occurence #
  unsigned int freq;
  unsigned int orgFreq;
  std::string flags;
  bool offensive;
  bool android;

  void iCount(){
    count++;
  }
};


struct {
  bool operator()(const structWord& a, const structWord& b){
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
  bool operator()(const structWord& a, const structWord& b){
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

#endif // STRUCT_WORD_H ///:~
