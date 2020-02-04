
#include "logger.h"
#include <iostream>

void raft::Logger::Info(const std::string& msg) {
  std::cout << GetCurTime() << ": " << msg << std::endl;
}

void raft::Logger::Err(const std::string& msg) {
  std::cout << GetCurTime() << ": " << msg << std::endl;
}

std::string raft::Logger::GetCurTime() {
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
  return std::string(buffer);
}
