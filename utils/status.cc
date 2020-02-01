
#include "status.h"

raft::Status::Status(Code c, const std::string& msg = ""): code_(c), msg_(msg) {

}
bool raft::Status::ok() {
  return code_ == Code::OK;
}
raft::Status raft::Status::OK() {
  return Status(Code::OK);
}
