
#include "status.h"

#include "utils.h"

raft::Status::Status(Code c, const std::string& msg = ""): code_(c), msg_(msg) {

}
bool raft::Status::ok() {
  return code_ == Code::OK;
}

std::string raft::Status::ToString() {
  return Utils::StringFormat("msg: %s, code: %d", msg_.c_str(), code_);
}
raft::Status raft::Status::OK() {
  return Status(Code::OK);
}

raft::Status raft::Status::Error(const std::string& msg) {
  return Status(Code::ERROR, msg);
}

raft::Status raft::Status::InvalidFormat(const std::string& msg) {
  return Status(Code::INVALID_FORMAT, msg);
}

raft::Status raft::Status::InvalidArg(const std::string& msg) {
  return Status(Code::INVALID_ARG, msg);
}

raft::Status raft::Status::RejectRequest(const std::string& msg) {
  return Status(Code::REJECT, msg);
}
