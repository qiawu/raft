#ifndef STATUS_H
#define STATUS_H

#include<string>

namespace raft {
  enum class Code {
    OK,
    ERROR
  };

  class Status {
    public:
      Status(Code c, const std::string& msg);
      bool ok();
      static Status OK();
    private:
      Code code_;
      std::string msg_;
  };

}

#endif
