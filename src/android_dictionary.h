#ifndef ANDROID_DICTIONARY_H
#define ANDROID_DICTIONARY_H
#pragma once

#include <map>
#include <string>
#include <vector>

#include "struct.words.h"
#include "wordcount.h"

class Android_Dictionary{
  private:
    std::map<std::string, std::vector<structWordcount>> dictionary;

  public:
    Android_Dictionary();
    ~Android_Dictionary();

    void open();
    void add_words(Wordcount *r_Wordcount);
};


#endif // ANDROID_DICTIONARY_H ///:~
