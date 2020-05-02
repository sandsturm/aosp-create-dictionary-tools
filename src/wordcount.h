#ifndef WORDCOUNT_H
#define WORDCOUNT_H

struct WordCount {
  std::string word;    // Word
  int count;      // Occurence #

  void iCount(){
    count++;
  }

  WordCount(std::string s){
    word = s;
    count = 1;
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

#endif // WORDCOUNT_H ///:~
