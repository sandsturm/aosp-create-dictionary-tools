#ifndef WORDCOUNT_H
#define WORDCOUNT_H

struct WordCount {
  std::string word;    // Word
  unsigned int count;      // Occurence #
  unsigned int freq;
  unsigned int orgFreq;
  std::string flags;
  bool offensive;

  void iCount(){
    count++;
  }

  WordCount(std::string s){
    word = s;
    count = 1;
    flags = "";
    offensive = false;
  }

  WordCount(std::string s, unsigned int fr, std::string fl, unsigned int org, bool o){
    word = s;
    count = 1;
    freq = fr;
    flags = fl;
    orgFreq = org;
    offensive = o;
  }
};

struct {
  bool operator()(const WordCount& a, const WordCount& b){
    if (a.count < b.count)
        return false;
    else if (a.count > b.count)
        return true;
    else{
        if (a.word < b.word)
            return true;
        else
            return false;
    }
  }
} CompareWordCount;

struct {
  bool operator()(const WordCount& a, const WordCount& b){
    if (a.freq < b.freq)
        return false;
    else if (a.freq > b.freq)
        return true;
    else{
        if (a.word < b.word)
            return true;
        else
            return false;
    }
  }
} CompareFrequency;

#endif // WORDCOUNT_H ///:~
