
#ifndef CANDIDATE_HANDLER_H
#define CANDIDATE_HANDLER_H

#include "raft_handler.h"
#include "replicate_log_manager.h"

namespace raft {
  class RaftNode;

  class CandidateHandler: public RaftHandler {
    public:
      CandidateHandler(RaftNode* node, ReplicateLogManager* log_manager): RaftHandler(node, log_manager, HandlerType::Candidate) {}
      Status Init() override;
      Message HandleMessage(const Message& req) override;
      ~CandidateHandler();
    protected:
      Status AddReplicateLog() override;
      Status CommitReplicateLog() override;
    private:
      Status StartElection();
      void OnVoteResult(const Message& msg);
  };
}

#endif
