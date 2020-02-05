
#ifndef CANDIDATE_HANDLER_H
#define CANDIDATE_HANDLER_H

#include <memory>
#include <map>

#include "raft_handler.h"
#include "replicate_log_manager.h"
#include "network/raft_client.h"
#include "utils/timer.h"

namespace raft {
  class RaftNode;

  class CandidateHandler: public RaftHandler {
    public:
      CandidateHandler(RaftNode* node, ReplicateLogManager* log_manager): 
        RaftHandler(node, log_manager, Membership::Candidate),
        is_in_election_(false),
        vote_for_me_cnt_(0),
        vote_for_other_cnt_(0),
        vote_counter_() {}
        // TODO: support sharing same queue for all clients
        //receive_queue_() {}
      Status Init() override;
      ~CandidateHandler();
    protected:
      Status ProcessMessage(const Message* req, ResponseCBFunc cb) override;
      Status AddReplicateLog(const Message* req, ResponseCBFunc cb);

    private:

      Status StartElection();
      Status EndElection(bool elect_succeed);
      Status ResetTimerForElection();

      // Someone asks me to vote. 
      // The handler when I got a vote request
      Status OnAskForVote(const Message* req, ResponseCBFunc cb);

      // I am asking for vote. 
      // The callback that when I got some vote result
      void OnVoteResult(const Message* resp);
      Status CountVoteResult(const Message* vote_result);
      bool IsValidVote(const VoteResponseMessage* vote);

      bool is_in_election_;
      uint32_t vote_for_me_cnt_;
      uint32_t vote_for_other_cnt_;
      std::map<std::string, uint32_t> vote_counter_;

      Timer timer_;
      // TODO: support sharing same queue for all clients
      // the queue to be shared for all clients sending reqs to other nodes
      //std::shared_ptr<RaftClientQueue> receive_queue_;
  };
}

#endif
