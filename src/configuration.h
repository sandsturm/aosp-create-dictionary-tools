#ifndef CONFIGURATION_H_INCLUDE
#define CONFIGURATION_H_INCLUDE

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

#endif // CONFIGURATION_H_INCLUDE ///:~
