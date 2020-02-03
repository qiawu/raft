
#include "node_address.h"

#include "utils/status.h"
#include "utils/utils.h"

raft::Status raft::NodeAddress::ParseNodeAddress(const std::string& raw, raft::NodeAddress& out_addr) {
  auto ip_and_port = Utils::Split(raw, ":");
  if (ip_and_port.size() != 2) {
    return Status::InvalidArg(raw);
  }

  out_addr.ip_ = ip_and_port[0];
  out_addr.port_ = ip_and_port[1];
  return Status::OK();
}

std::string raft::NodeAddress::ToString() const {
  return Utils::StringFormat("%s:%s", ip_.c_str(), port_.c_str());
}
