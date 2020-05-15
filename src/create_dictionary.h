#include <fstream>
#include <iostream>
#include <chrono>
#include <map>
#include <vector>

#include "android_dictionary.h"
#include "spellcheck.h"
#include "wordcount.h"

class Dictionary{
  private:
    // Files
    std::ifstream m_InputFile;

    // Variables
    std::string line;
    long count;

    // Initiate objects
    Android_Dictionary m_AndroidDictionary;
    Spellcheck m_Spellcheck;
    Wordcount m_Wordcount;

  public:
    // The Dictionary constructor
    Dictionary();
    ~Dictionary();

    // Run will call all the private functions
    void run(std::string inputFile);

    void print_status(std::string status, std::string buffer);

    std::string Normalize(std::string s);
};
