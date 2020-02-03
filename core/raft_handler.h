
#ifndef RAFT_HANDLER_H
#define RAFT_HANDLER_H

#include <memory>

#include "utils/status.h"

namespace raft {
  class RaftNode;

  enum class HandlerType {
    Leader,
    Follower,
    Candidate
  };
  class RaftHandler {
    public:
      RaftHandler(RaftNode* node, HandlerType type);
      virtual Status Init() = 0;
      virtual Status HandleMessage() = 0;
      virtual ~RaftHandler();
      
      HandlerType GetType() { return type_; }

    protected:
      virtual Status AddReplicateLog() = 0;
      virtual Status CommitReplicateLog() = 0;
      HandlerType type_;
      RaftNode* host_node_;
  };
}

#endif
