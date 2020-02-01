
#ifndef RAFT_NODE_H
#define RAFT_NODE_H

#include "grpc/raft_server.h"
#include "grpc/raft_client.h"
#include "utils/status.h"
#include "raft_handler.h"
#include "node_identity.h"
#include "message.h"

namespace raft {
  class RaftNode {
    public:
      RaftNode(); 
      Status Initialize(const std::string& conf_path);
      // this is the only entry to invoke handler
      Status OnMessage(const Message& msg);
      // this is the only entry the handler can talk to RaftNode
      Status SendMessage(const NodeIdentify& receiver, uint32_t sec_timeout, uint32_t retry_times);
      
      Status SwitchMemship(HandlerType src, HandlerType tgt);
    private:
      Status LoadConf(const std::string& conf_path);

      RaftHandler* handler_;
      RaftClient* grpc_client_;
      RaftServiceImpl* grpc_server_;
  };
}

#endif
