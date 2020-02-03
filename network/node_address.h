#ifndef NODE_ADDRESS_H
#define NODE_ADDRESS_H 

#include <string>

#include "utils/status.h"

namespace raft {
  class NodeAddress {
    public:
      NodeAddress(): ip_("invalid"), port_("invalid") {}
      static Status ParseNodeAddress(const std::string& raw, raft::NodeAddress& out_addr);

      std::string GetIP() { return ip_; }
      std::string GetPort() { return port_; }
      std::string ToString() const;
    private:
      std::string ip_;
      std::string port_;
  };
}
#endif /* NODE_ADDRESS_H */
