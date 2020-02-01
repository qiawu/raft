
#ifndef CANDIDATE_HANDLER_H
#define CANDIDATE_HANDLER_H

#include "raft_handler.h"

namespace raft {
  class RaftNode;

  class CandidateHandler: public RaftHandler {
    public:
      CandidateHandler(RaftNode* node): RaftHandler(node, HandlerType::Candidate) {}
      Status Init() override;
      Status HandleMessage() override;
      ~CandidateHandler();
    protected:
      Status AddReplicateLog() override;
      Status CommitReplicateLog() override;
  };
}

#endif
