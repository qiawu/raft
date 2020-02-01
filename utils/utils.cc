
#include "utils.h"

#include <stdarg.h>

#include <memory>
#include <string>
#include <stdexcept>
#include <vector>
#include <regex>

std::string raft::Utils::StringFormat(const std::string fmt, ...)
{
      int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
    std::string str;
    va_list ap;
    while (1) {     // Maximum two passes on a POSIX system...
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
        va_end(ap);
        if (n > -1 && n < size) {  // Everything worked
            str.resize(n);
            return str;
        }
        if (n > -1)  // Needed size returned
            size = n + 1;   // For null char
        else
            size *= 2;      // Guess at a larger size (OS specific)
    }
    return str;
}

std::vector<std::string> raft::Utils::Split(const std::string& input, const std::string& regex) {
  const std::regex re(regex);
  std::vector<std::string> container(
      std::sregex_token_iterator(input.begin(), input.end(), re, -1), 
      std::sregex_token_iterator()
      );
  return container;
}

std::string raft::Utils::Trim(const std::string &str) {
  size_t first = str.find_first_not_of(' ');
  if (std::string::npos == first)
  {
    return str;
  }
  size_t last = str.find_last_not_of(' ');
  return str.substr(first, (last - first + 1));
}
