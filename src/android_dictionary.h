#ifndef ANDROID_DICTIONARY_H
#define ANDROID_DICTIONARY_H
#pragma once

#include <map>
#include <string>
#include <vector>

#include "struct.word.h"
#include "dictionary.h"

class Android_Dictionary{
  private:
    std::map<std::string, std::vector<structWord>> dictionary;

  public:
    Android_Dictionary();
    ~Android_Dictionary();

    void open();
    void add_words(Dictionary *r_Dictionary);
};


#endif // ANDROID_DICTIONARY_H ///:~
