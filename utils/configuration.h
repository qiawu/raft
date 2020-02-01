#ifndef CONFIGURATION_H
#define CONFIGURATION_H 

#include <vector>
#include <string>
#include <map>

#include "utils/status.h"

namespace raft {
  class Configuration {
    public:
      Configuration(const std::string& conf): conf_path_(conf) {}
      Status Parse();
      void PrettyPrint();
    private:
      std::string conf_path_;
      std::vector<std::string> sections_;
      std::vector<std::map<std::string, std::string>> props_;
  };
}
#endif /* CONFIGURATION_H */
