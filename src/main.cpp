#include <bits/stdc++.h>
#include <iostream>
#include <chrono>

inline bool isInteger(const std::string & s)
{
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

   char * p;
   strtol(s.c_str(), &p, 10);

   return (*p == 0);
}

int main() {
  // Open the dictionary file to read data
  std::ifstream inputFile("demodata/de_full.txt");
  std::string line;
  long count;

  // Count every line of the file
  for (count = 0; std::getline(inputFile, line); ++count)
  ;

  // Calculate the divider to ensure results between 50 and 254
  // int divider = int (count / 205) + 1;
  int divider = int(count / 205) + 1;

  // Return to the beginning of the input file
  inputFile.clear();
  inputFile.seekg(0);

  // Open the new dictionary file to write data
  std::ofstream outputFile("demodata/output.txt");

  // Write version and timestamp
  // Format: dictionary=main:de,locale=de,description=Deutsch,date=1414726263,version=54,REQUIRES_GERMAN_UMLAUT_PROCESSING=1
  std::string strLocale = "de";
  std::string strDescription = "Deutsch";
  std::string strVersion = "54";

  outputFile << "dictionary=main:" << strLocale
             << ",locale=" << strLocale
             << ",description=" << strDescription
             << ",date=" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
             << ",version=" << strVersion
             << ",REQUIRES_GERMAN_UMLAUT_PROCESSING=1"
             << '\n';

  // Variables for storing data from file
  std::string dictWord;
  std::string dictCount;

  // Iterate through dict file from top to bottom
  for (long i = 0; std::getline(inputFile, line); ++i){
    count--;
    // std::getline(inputFile, line);
    dictWord = line.substr(0, line.find(" "));
    dictCount = line.substr(line.find(" ") + 1);
    if (isInteger(dictCount)){
      int frequency = (count / divider) + 50;
      // Format: word=der,f=216,flags=,originalFreq=216
      outputFile << " word=" << dictWord << ",f=" << frequency << ",flags=,originalFreq=" << frequency << '\n';
    }
  }

  // Close files
  inputFile.close();
  outputFile.close();

  return 0;
}
