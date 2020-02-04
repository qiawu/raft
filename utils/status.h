#ifndef STATUS_H
#define STATUS_H

#include<string>

namespace raft {
  enum class Code {
    OK,
    ERROR,
    INVALID_FORMAT,
    INVALID_ARG,
    REJECT
  };

  class Status {
    public:
      Status(): code_(Code::OK), msg_("") {}
      Status(Code c, const std::string& msg);
      bool ok();
      std::string ToString();
      static Status OK();
      static Status Error(const std::string& msg);
      static Status InvalidFormat(const std::string& msg);
      static Status InvalidArg(const std::string& msg);
      static Status RejectRequest(const std::string& msg);
    private:
      Code code_;
      std::string msg_;
  };

}

#endif
