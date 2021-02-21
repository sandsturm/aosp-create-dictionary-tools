#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "dictionary.h"
#include "spellcheck.h"

Spellcheck::Spellcheck(){
  // m_MinWordcount = min;
  m_MinWordcount = 50;
}

Spellcheck::~Spellcheck(){
}

int Spellcheck::calc_queue(int character){
  if(character < 0){
    return 0;
  } else if(character > 90){
    return (character - 97) * m_Workers / 27;
  } else {
    return (character - 65) * m_Workers / 27;
  }
}

void Spellcheck::set(int id, int workers){
  m_Id = id;
  m_Workers = workers;
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
      if(token.length() > 0){
        subs = token;
      }

      if(calc_queue(int(subs.front())) == m_Id){
        spellcheck.insert(subs);
      }
    } while (iss);
  }
  // Close spellcheckFile since everthing is in the spellcheck vector
  m_SpellcheckFile.close();
}

void Spellcheck::missing(std::string s){
  std::map<std::string, unsigned int>::iterator it;

  it = m_MissingSpellcheck.find(s);
  if(it != m_MissingSpellcheck.end()){
    m_MissingSpellcheck[s] = it->second + 1;
  } else {
    m_MissingSpellcheck[s] = 1;
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
  for (it = m_MissingSpellcheck.begin(); it != m_MissingSpellcheck.end(); it++)
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

  std::cout << "Writing file with words missing in spellcheck file." << '\n';
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

void Spellcheck::append(std::map<std::string, unsigned int> missing){
  m_MissingSpellcheck.insert(missing.begin(), missing.end());
}

std::map<std::string, unsigned int> Spellcheck::getMissingSpellcheck(){
  return m_MissingSpellcheck;
}
