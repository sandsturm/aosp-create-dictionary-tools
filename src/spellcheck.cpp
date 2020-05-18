#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

#import "spellcheck.h"

Spellcheck::Spellcheck(unsigned int min){
  min_wordcount = min;
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

void Spellcheck::exportFile(){
  // Export missing m_Spellcheck
  // Open file to write missing m_Spellcheck
  std::ofstream missesFile("demodata/missing_in_spellcheck.txt");

  std::map<std::string, unsigned int>::iterator it;

  for(it = missingSpellcheck.begin(); it != missingSpellcheck.end(); it++){
    if(it->second > min_wordcount){
      missesFile << it->first << '\n';
    }
  }

  missesFile.close();
}
