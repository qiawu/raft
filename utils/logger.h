#ifndef LOGGER_H
#define LOGGER_H 

#include <string>

namespace raft {
  class Logger {
    public:
      static void Log(const std::string& msg);
    private:
      Logger() {}
      static std::string GetCurTime();
  };
}
#endif /* LOGGER_H */
