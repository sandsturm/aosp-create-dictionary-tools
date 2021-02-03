#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "dictionary.h"
#include "spellcheck.h"

Spellcheck::Spellcheck(unsigned int min){
  m_MinWordcount = min;
}

Spellcheck::~Spellcheck(){
}

Spellcheck::Spellcheck(std::string s){
    spellcheck.insert(s);
}

bool Spellcheck::find(std::string s){
  return spellcheck.find(s) != spellcheck.end();
}

void Spellcheck::insert(std::string s){
  spellcheck.insert(s);
}

void Spellcheck::open(std::string s){
  // Open the spellcheck file
  std::ifstream m_SpellcheckFile;

  // Read spellcheck file
  m_SpellcheckFile.open(s);

  std::string line;

  // Iterate through spellcheck file
  for (long i = 0; std::getline(m_SpellcheckFile, line); ++i){
    std::istringstream iss(line);
    std::string delimiter = "/";

    do{
      std::string subs;
      iss >> subs;
      // TODO Check token!
      // Check if the / delimiter is in the line.
      // There are some lines without delimiter.
      std::string token = subs.substr(0, subs.find(delimiter));
      if (token.length() > 0){
        subs = token;
      }
      spellcheck.insert(subs);
    } while (iss);
  }

  // Close spellcheckFile since everthing is in the spellcheck vector
  std::cout << "Spellcheck file loaded." << '\n';
  m_SpellcheckFile.close();
}

void Spellcheck::missing(std::string s){
  std::map<std::string, unsigned int>::iterator it;

  it = missingSpellcheck.find(s);
  if(it != missingSpellcheck.end()){
    missingSpellcheck[s] = it->second + 1;
  } else {
    missingSpellcheck[s] = 1;
  }
}

void Spellcheck::exportFile(Dictionary dictionary){
  // Export missing m_Spellcheck
  // Open file to write missing m_Spellcheck
  std::ofstream missesSpellcheckFile("demodata/missing_in_spellcheck.txt");
  std::ofstream missesDictionaryFile("demodata/missing_in_spellcheck_dictionary.txt");

  std::map<std::string, unsigned int>::iterator it;

  // Copy missing words map contain to vector for easier sorting
  std::vector<std::pair<std::string, unsigned int>> words;
  for (it = missingSpellcheck.begin(); it != missingSpellcheck.end(); it++)
      words.push_back(*it);

  // Sort pairs container
  sort(words.begin(), words.end(), [=](std::pair<std::string, unsigned int>& a, std::pair<std::string, unsigned int>& b){
    return a.second > b.second;
  });

  // Read from configuration file how many missing words should be exported (in percent)
  unsigned int min_wordcount = words.size() * m_MinWordcount / 100;
  std::cout << "Minimum wordcount: " << min_wordcount << '\n';

  std::vector<std::pair<std::string, unsigned int>>::iterator iter;
  unsigned int count = 0;

  for(iter = words.begin(); (iter != words.end() && count < min_wordcount) ; iter++){
    count++;
    missesSpellcheckFile << iter->first << '\n';
    if(dictionary.rowWord(iter->first) < 0){
      missesDictionaryFile << iter->first << '\n';
    }
  }

  std::cout << "Spellcheck file exported." << '\n';

  missesSpellcheckFile.close();
  missesDictionaryFile.close();
}
