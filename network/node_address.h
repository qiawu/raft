#ifndef NODE_ADDRESS_H
#define NODE_ADDRESS_H 

#include <string>

#include "utils/status.h"

namespace raft {
  class NodeAddress {
    public:
      static Status ParseNodeAddress(const std::string& raw, raft::NodeAddress& out_addr);

      std::string GetIP() { return ip_; }
      std::string GetPort() { return port_; }
      std::string ToString();
    private:
      std::string ip_;
      std::string port_;
  };
}
#endif /* NODE_ADDRESS_H */
