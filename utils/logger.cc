
#include "logger.h"
#include <iostream>
#include <time.h>

#include "configuration.h"

std::mutex raft::Logger::mutex_;
std::unique_ptr<std::string> raft::Logger::log_path_(nullptr);

void raft::Logger::SetLogPath(const std::string& path) {
  log_path_ = std::unique_ptr<std::string>(new std::string(path));
}
void raft::Logger::Info(const std::string& msg) {
  const std::lock_guard<std::mutex> lock(mutex_);
  std::cout << GetCurTime() << ": " << msg << std::endl;
}

void raft::Logger::Err(const std::string& msg) {
  const std::lock_guard<std::mutex> lock(mutex_);
  std::cout << GetCurTime() << ": " << msg << std::endl;
}

void raft::Logger::Debug(const std::string& msg) {
  const std::lock_guard<std::mutex> lock(mutex_);
  std::cout << GetCurTime() << ": " << msg << std::endl;
}

std::string raft::Logger::GetCurTime() {
  char buff[100];
  time_t now = time (0);
  strftime (buff, 100, "%Y-%m-%d %H:%M:%S.000", localtime (&now));
  return std::string(buff);
}
