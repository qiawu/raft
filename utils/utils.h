#ifndef UTILS_H
#define UTILS_H 

#include <string>
#include <algorithm>
#include <cctype>
#include <locale>
#include <vector>

namespace raft {
  class Utils {
    public:
      static std::string StringFormat(const std::string fmt, ...);
      static std::vector<std::string> Split(const std::string& input, const std::string& regex);
      static std::string Trim(const std::string &str);

  };
}
#endif /* UTILS_H */
