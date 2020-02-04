#ifndef LOGGER_H
#define LOGGER_H 

#include <string>

namespace raft {
  class Logger {
    public:
      static void Info(const std::string& msg);
      static void Err(const std::string& msg);
    private:
      Logger() {}
      static std::string GetCurTime();
  };
}
#endif /* LOGGER_H */
