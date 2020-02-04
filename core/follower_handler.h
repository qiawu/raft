
#ifndef FOLLOWER_HANDLER_H
#define FOLLOWER_HANDLER_H

#include "raft_handler.h"
#include "utils/timer.h"

namespace raft {
  class RaftNode;

  class FollowerHandler: public RaftHandler {
    public:
      FollowerHandler(RaftNode* node, ReplicateLogManager* log_manager): 
        RaftHandler(node, log_manager, Membership::Follower) {}
      Status Init() override;
      ~FollowerHandler();

    protected:
      Status ProcessMessage(const Message* req, ResponseCBFunc cb) override;
      Status AddReplicateLog(const Message* req, ResponseCBFunc cb);

    private:
      // Someone asks me to vote. 
      // The handler when I got a vote request
      Status OnAskForVote(const Message* req, ResponseCBFunc cb);

      Status StartElection();
      Status CommitReplicateLog();

      Timer timer_;
  };
}

#endif
