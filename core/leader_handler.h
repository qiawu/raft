
#ifndef LEADER_HANDLER_H
#define LEADER_HANDLER_H

#include "raft_handler.h"

namespace raft {
  class RaftNode;

  class LeaderHandler : public RaftHandler {
    public:
      LeaderHandler(RaftNode* node, ReplicateLogManager* log_manager): RaftHandler(node, log_manager, HandlerType::Leader) {}
      Status Init() override;
      Message HandleMessage(const Message& req) override;
      ~LeaderHandler();

    protected:
      Status AddReplicateLog() override;
      Status CommitReplicateLog() override;
  };
}

#endif
