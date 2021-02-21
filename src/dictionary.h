#ifndef DICTIONARY_H_INCLUDE
#define DICTIONARY_H_INCLUDE

#include <set>
#include <string>
#include <vector>

#include "struct.word.h"

class Dictionary{
  private:
    const unsigned int DIVIDER = 90;
    const unsigned int BASEFREQ = 1;

    std::vector<structWord> dictionary;

    int m_Id;
    int m_Workers;

  public:
    Dictionary();
    ~Dictionary();

    Dictionary(std::string s);
    Dictionary(std::string s, unsigned int fr, std::string fl, unsigned int org, bool o);

    void set(int id, int workers);
    void addWord(std::string s);
    void addFrequency();
    void iCount();
    bool findWord(std::string s);
    long rowWord(std::string s);
    void loadDict(std::string fileName, bool custom = false);
    void sortCount();
    void sortFrequency();

    void append(std::vector<structWord> externdict);
    std::vector<structWord> getDictionaryEntries();

    void exportFile();
};

#endif // DICTIONARY_H_INCLUDE ///:~
