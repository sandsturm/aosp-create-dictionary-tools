#ifndef SPELLCHECK_H
#define SPELLCHECK_H
#pragma once

#include <fstream>
#include <map>
#include <string>
#include <set>

class Spellcheck{
  private:
    // Set with words for spellchecking input file
    std::set<std::string> spellcheck;

    // Set with words which are in the input file but are missing in the spellcheck file
    std::map<std::string, unsigned int> missingSpellcheck;

    // Minimum number of hits to export to missing spellcheck file
    unsigned int m_MinWordcount;

  public:
    Spellcheck(unsigned int min);
    ~Spellcheck();

    Spellcheck(std::string s);

    void exportFile();
    bool find(std::string s);
    void insert(std::string s);
    void missing(std::string s);
    void open(std::string s);
};

#endif // SPELLCHECK_H ///:~
