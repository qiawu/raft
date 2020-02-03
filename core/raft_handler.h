
#ifndef RAFT_HANDLER_H
#define RAFT_HANDLER_H

#include <memory>

#include "replicate_log_manager.h"
#include "utils/status.h"
#include "network/message.h"

namespace raft {
  class RaftNode;

  enum class HandlerType {
    Leader,
    Follower,
    Candidate
  };
  class RaftHandler {
    public:
      RaftHandler(RaftNode* node, ReplicateLogManager* log_manager, HandlerType type);
      virtual Status Init() = 0;
      virtual Message HandleMessage(const Message& req) = 0;
      virtual ~RaftHandler();
      
      HandlerType GetType() { return type_; }

      static const uint32_t ElectionMillSecTimeOut = 150;
    protected:
      virtual Status AddReplicateLog() = 0;
      virtual Status CommitReplicateLog() = 0;
      RaftNode* host_node_;
      ReplicateLogManager* log_manager_;
      HandlerType type_;
  };
}

#endif
