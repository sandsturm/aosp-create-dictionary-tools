#ifndef WORDCOUNT_H
#define WORDCOUNT_H
#pragma once

#include <set>
#include <string>
#include <vector>

#include "struct.word.h"

class Wordcount{
  private:
    std::vector<structWord> wordcount;

  public:
    Wordcount();
    ~Wordcount();

    Wordcount(std::string s);
    Wordcount(std::string s, unsigned int fr, std::string fl, unsigned int org, bool o);
    void addWord(std::string s);
    void addWordcount(std::string s, unsigned int fr, std::string fl, unsigned int org, bool o);
    void updateWord(long i, unsigned int fr, std::string fl, unsigned int org, bool o);
    void addFrequency();
    void iCount();
    bool findWord(std::string s);
    long rowWord(std::string s);
    void exportFile();
    void sort();
};

#endif // WORDCOUNT_H ///:~
