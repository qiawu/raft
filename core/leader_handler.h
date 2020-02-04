
#ifndef LEADER_HANDLER_H
#define LEADER_HANDLER_H

#include "raft_handler.h"

namespace raft {
  class RaftNode;

  class LeaderHandler : public RaftHandler {
    public:
      LeaderHandler(RaftNode* node, ReplicateLogManager* log_manager): RaftHandler(node, log_manager, Membership::Leader) {}
      Status Init() override;
      ~LeaderHandler();

    protected:
      Status ProcessMessage(const Message* req, ResponseCBFunc cb) override;
      Status AddReplicateLog(const Message* req, ResponseCBFunc cb);

    private:
      // Someone asks me to vote. 
      // The handler when I got a vote request
      Status OnAskForVote(const Message* req, ResponseCBFunc cb);

      Status CommitReplicateLog();
  };
}

#endif
