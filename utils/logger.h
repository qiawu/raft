#ifndef LOGGER_H
#define LOGGER_H 

#include <string>
#include <mutex>

namespace raft {
  class Logger {
    public:
      static void SetLogPath(const std::string& path);
      static void Info(const std::string& msg);
      static void Err(const std::string& msg);
      static void Debug(const std::string& msg);
    private:
      Logger() {}
      static std::string GetCurTime();
      static std::mutex mutex_;
      static std::unique_ptr<std::string> log_path_;
  };
}
#endif /* LOGGER_H */
