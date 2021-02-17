#ifndef DICTIONARY_H
#define DICTIONARY_H
#pragma once

#include <set>
#include <string>
#include <vector>

#include "struct.word.h"

class Dictionary{
  private:
    const unsigned int DIVIDER = 90;
    const unsigned int BASEFREQ = 1;

    std::vector<structWord> dictionary;

  public:
    Dictionary();
    ~Dictionary();

    Dictionary(std::string s);
    Dictionary(std::string s, unsigned int fr, std::string fl, unsigned int org, bool o);
    void addWord(std::string s);
    void addFrequency();
    void iCount();
    bool findWord(std::string s);
    long rowWord(std::string s);
    void exportFile();
    void loadDict(std::string fileName);
    void sortCount();
    void sortFrequency();

    void append(std::vector<structWord> missing);
    std::vector<structWord> getDictionaryEntries();
};

#endif // DICTIONARY_H ///:~
