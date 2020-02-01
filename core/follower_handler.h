
#ifndef FOLLOWER_HANDLER_H
#define FOLLOWER_HANDLER_H

#include "raft_handler.h"

namespace raft {
  class RaftNode;

  class FollowerHandler: public RaftHandler {
    public:
      FollowerHandler(RaftNode* node): RaftHandler(node, HandlerType::Follower) {}
      Status Init() override;
      Status HandleMessage() override;
      ~FollowerHandler();

    protected:
      Status AddReplicateLog() override;
      Status CommitReplicateLog() override;
  };
}

#endif
