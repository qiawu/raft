
#ifndef LEADER_HANDLER_H
#define LEADER_HANDLER_H

#include "raft_handler.h"

namespace raft {
  class RaftNode;

  class LeaderHandler : public RaftHandler {
    public:
      LeaderHandler(RaftNode* node): RaftHandler(node, HandlerType::Leader) {}
      Status Init() override;
      Status HandleMessage() override;
      ~LeaderHandler();

    protected:
      Status AddReplicateLog() override;
      Status CommitReplicateLog() override;
  };
}

#endif
