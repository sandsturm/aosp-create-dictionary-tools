#ifndef SPELLCHECK_H
#define SPELLCHECK_H
#pragma once

#include <fstream>
#include <string>
#include <set>

class Spellcheck{
  private:
    // Set with words for spellchecking input file
    std::set<std::string> spellcheck;

    // Set with words which are in the input file but are missing in the spellcheck file
    std::set<std::string> missingSpellcheck;

  public:
    Spellcheck();
    ~Spellcheck();

    Spellcheck(std::string s);

    void exportFile();
    bool find(std::string s);
    void insert(std::string s);
    void missing(std::string s);
    void open();
};

#endif // SPELLCHECK_H ///:~
