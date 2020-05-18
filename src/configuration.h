#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#pragma once

#include <map>
#include <string>

class Configuration{
  private:
    std::map<std::string, std::string> configuration;

  public:
    Configuration();
    ~Configuration();

    std::string value(std::string s);
};

#endif // CONFIGURATION_H ///:~
