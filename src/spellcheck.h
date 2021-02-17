#ifndef SPELLCHECK_H
#define SPELLCHECK_H
#pragma once

#include <fstream>
#include <map>
#include <set>
#include <string>

#include "dictionary.h"

class Spellcheck{
  private:
    // Set with words for spellchecking input file
    std::set<std::string> spellcheck;

    // Set with words which are in the input file but are missing in the spellcheck file
    std::map<std::string, unsigned int> m_MissingSpellcheck;

    int m_Id;
    int m_Workers;

    // Minimum number of hits to export to missing spellcheck file
    unsigned int m_MinWordcount;

  public:
    Spellcheck();
    ~Spellcheck();

    void set(int id, int workers);
    void exportFile(Dictionary dictionary);
    bool find(std::string s);
    void insert(std::string s);
    void missing(std::string s);
    void open(std::string s);

    void append(std::map<std::string, unsigned int> missing);
    std::map<std::string, unsigned int> getMissingSpellcheck();
};

#endif // SPELLCHECK_H ///:~
