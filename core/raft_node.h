
#ifndef RAFT_NODE_H
#define RAFT_NODE_H

#include "network/raft_server.h"
#include "network/raft_client.h"
#include "network/node_address.h"
#include "utils/status.h"
#include "raft_handler.h"
#include "node_identity.h"
#include "message.h"

namespace raft {
  class RaftNode {
    public:
      RaftNode(): handler_(nullptr), raft_client_(nullptr), raft_server_(nullptr), node_list_(), local_name_() {} 
      ~RaftNode();
      Status Initialize(const std::string& conf_path);
      // this is the only entry to invoke handler
      Status OnMessage(const Message& msg);
      // this is the only entry the handler can talk to RaftNode
      Status SendMessage(const NodeIdentify& receiver, uint32_t sec_timeout, uint32_t retry_times);
      
      Status SwitchMemship(HandlerType src, HandlerType tgt);
    private:
      Status LoadConf(const std::string& conf_path);

      RaftHandler* handler_;
      RaftClient* raft_client_;
      RaftServer* raft_server_;

      std::map<std::string, NodeAddress> node_list_;
      std::string local_name_;
  };
}

#endif
