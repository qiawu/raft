
#ifndef RAFT_NODE_H
#define RAFT_NODE_H

#include <memory>
#include <atomic>
#include <thread>

#include "network/raft_server.h"
#include "network/raft_client.h"
#include "network/node_address.h"
#include "network/message.h"
#include "utils/status.h"
#include "utils/blocking_queue.h"
#include "raft_handler.h"
#include "node_identity.h"

namespace raft {
  class RaftNode {
    public:
      RaftNode(): is_node_shutting_down_(false), handler_(nullptr), raft_client_(nullptr), raft_server_(nullptr), node_list_(), local_name_() {} 
      ~RaftNode();
      Status Initialize(const std::string& conf_path);
      void ProcessMessages();
      // submit to queue
      Status SubmitMessage(Message* msg);
      // this is the only entry to invoke handler
      Status HandleMessage(const Message& msg);
      // this is the only entry the handler can talk to RaftNode
      Status SendMessage(const NodeIdentify& receiver, uint32_t sec_timeout, uint32_t retry_times);
      
      Status SwitchMemship(HandlerType src, HandlerType tgt);
    private:
      Status LoadConf(const std::string& conf_path);

      BlockingQueue<Message*> msg_queue_;
      std::thread bg_msg_processor_;
      std::atomic<bool> is_node_shutting_down_;

      std::unique_ptr<RaftHandler> handler_;
      std::unique_ptr<RaftClient> raft_client_;
      std::unique_ptr<RaftServer> raft_server_;

      std::map<std::string, NodeAddress> node_list_;
      std::string local_name_;
  };
}

#endif
