
#ifndef RAFT_NODE_H
#define RAFT_NODE_H

#include <memory>
#include <atomic>
#include <thread>

#include "network/raft_server.h"
#include "network/raft_client.h"
#include "network/node_address.h"
#include "network/message.h"
#include "network/call_data.h"
#include "utils/status.h"
#include "utils/blocking_queue.h"
#include "raft_handler.h"
#include "replicate_log_manager.h"

namespace raft {
  struct QueueItem {
    public:
      QueueItem(): msg_(nullptr), cb_(nullptr) {}
      QueueItem(Message* msg, ResponseCBFunc cb): msg_(msg), cb_(cb) {}
      Message* msg_;
      ResponseCBFunc cb_;
  };

  class RaftNode {
    public:
      RaftNode(): is_node_shutting_down_(false), handler_(nullptr), raft_server_(nullptr), node_list_(), local_name_() {} 
      ~RaftNode();
      Status Initialize(const std::string& conf_path);
      void ProcessMessages();
      // submit to queue
      Status SubmitMessage(Message* msg, ResponseCBFunc cb);
      // this is the only entry to invoke handler
      Message HandleMessage(const Message& req);
      
      uint32_t GetMajority();
      std::string GetNodeName() {
        return local_name_;
      }
      std::map<std::string, NodeAddress> GetNodeList() {
        return node_list_;
      }
    private:
      Status LoadConf(const std::string& conf_path);
      Status InitReplicateLog();
      Status InitHandler();
      Status StartServer();
      Status StartMessageProcessor();
      Status SwitchMemship(HandlerType tgt);

      BlockingQueue<QueueItem> msg_queue_;
      std::thread bg_msg_processor_;
      std::atomic<bool> is_node_shutting_down_;

      ReplicateLogManager log_manager_;
      std::unique_ptr<RaftHandler> handler_;
      std::unique_ptr<RaftServer> raft_server_;

      std::map<std::string, NodeAddress> node_list_;
      std::string local_name_;
  };
}

#endif
