#include <sstream>
#include <string>

#include "configuration.h"

Configuration::Configuration(){
  const char configdata[] = ""
  // Export the top count of missing words (in percent)
  "minimum_missing_spellcheck=1\n"

  // Filename for words not found in spellcheck
  "filename_missing_spellcheck=missing_spellcheck.txt\n"

  // Spellcheck file with official dictionary e.g. from LibreOffice
  "spellcheck_dictionary=demodata/de_DE_frami.dic\n"
  // "spellcheck_dictionary=demodata/German_de_DE.dic\n"

  // Own (custom) spellcheck file
  "spellcheck_custom=demodata/own_spellcheck.txt";

  std::istringstream is_file(configdata);

  std::string line;

  while( std::getline(is_file, line) ){
    std::istringstream is_line(line);
    std::string key;
    if( std::getline(is_line, key, '=') ){
      std::string value;
      if( std::getline(is_line, value) ){
        configuration[key] = value;
      }
    }
  }
}

Configuration::~Configuration(){

}

std::string Configuration::value(std::string s){
  std::map<std::string, std::string>::iterator it;

  it = configuration.find(s);

  if(it != configuration.end()){
    return it->second;
  }
  return 0;
}
