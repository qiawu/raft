
#ifndef FOLLOWER_HANDLER_H
#define FOLLOWER_HANDLER_H

#include "raft_handler.h"

namespace raft {
  class RaftNode;

  class FollowerHandler: public RaftHandler {
    public:
      FollowerHandler(RaftNode* node, ReplicateLogManager* log_manager): RaftHandler(node, log_manager, HandlerType::Follower) {}
      Status Init() override;
      Message HandleMessage(const Message& req) override;
      ~FollowerHandler();

    protected:
      Status AddReplicateLog() override;
      Status CommitReplicateLog() override;
  };
}

#endif
