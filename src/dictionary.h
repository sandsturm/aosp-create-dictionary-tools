#ifndef DICTIONARY_H
#define DICTIONARY_H
#pragma once

#include <set>
#include <string>
#include <vector>

#include "struct.word.h"

class Dictionary{
  private:
    const int DIVIDER = 90;

    std::vector<structWord> dictionary;

  public:
    Dictionary();
    ~Dictionary();

    Dictionary(std::string s);
    Dictionary(std::string s, unsigned int fr, std::string fl, unsigned int org, bool o);
    void addWord(std::string s);
    void addWord(std::string s, unsigned int fr, std::string fl, unsigned int org, bool o);
    void updateWord(long i, unsigned int fr, std::string fl, unsigned int org, bool o);
    void addFrequency();
    void iCount();
    bool findWord(std::string s);
    long rowWord(std::string s);
    void exportFile();
    void loadAndroid();
    void sort();
};

#endif // DICTIONARY_H ///:~
