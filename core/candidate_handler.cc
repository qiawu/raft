
#include "candidate_handler.h"

#include "raft_node.h"
#include "utils/status.h"
#include "utils/logger.h"
#include "utils/utils.h"

raft::CandidateHandler::~CandidateHandler() {
  timer_.Stop();
}

raft::Status raft::CandidateHandler::Init() {
  return ResetTimerForElection();
}

raft::Status raft::CandidateHandler::ResetTimerForElection() {
  ElectionTimeoutMessage* msg = new ElectionTimeoutMessage("dummy");
  Timer t;
  t.SetTimeout([=](){
      this->host_node_->SubmitMessage(msg, [=](const Message*){ delete msg; });
      }, GenerateRandomElectionTimeout());
  return Status::OK();
}

raft::Status raft::CandidateHandler::ProcessMessage(const Message* req, ResponseCBFunc cb) {
  Status s;
  switch (req->type_) {
    case MessageType::ElectionTimeout:
      return StartElection();
    case MessageType::VoteResp:
      return CountVoteResult(req);
    case MessageType::VoteReq:
      return OnAskForVote(req, cb);
    case MessageType::ReplicateEntryReq:
      return AddReplicateLog(req, cb);
    default:
      return s;
  }
}

raft::Status raft::CandidateHandler::OnAskForVote(const Message* req, ResponseCBFunc cb) {
  // condition check:
  // 1. did I vote for others ?
  // 2. is he has more up-to-date logs
  
  const VoteRequestMessage* vote_req = dynamic_cast<const VoteRequestMessage*>(req);
  Logger::Debug(vote_req->ToString());
  if (!vote_req) {
    return Status::InvalidArg("failed to cast to VoteRequestMessage");
  }
  std::unique_ptr<Message> resp;
  // TODO: persist voted info, which should be done before sending back vote result (the sender should handle duplicate votes)
  if (vote_for_other_cnt_ >= 1) {
    resp = std::unique_ptr<Message>(new VoteResponseMessage(
          "I have voted for some", 
          vote_req->leader_cand_name_, 
          host_node_->GetNodeName(),
          vote_req->cur_term_, 
          vote_req->cur_index_, 
          false)
        );
    Logger::Debug(Utils::StringFormat("vote denied due to previous vote, cand: %s, voter: %s", vote_req->leader_cand_name_.c_str(), host_node_->GetNodeName().c_str()));
  } else {
    LogEntryPos caller_cur_pos = LogEntryPos(vote_req->cur_term_, vote_req->cur_index_);
    bool is_up_to_date = log_manager_->IsMoreUpdateToDate(caller_cur_pos);
    if (is_up_to_date) {
      vote_for_other_cnt_++;
      resp = std::unique_ptr<Message>(
          new VoteResponseMessage(
            "vote granted", 
            vote_req->leader_cand_name_, 
            host_node_->GetNodeName(),
            vote_req->cur_term_, 
            vote_req->cur_index_, 
            true)
          );
      Logger::Debug(Utils::StringFormat("vote granted, cand: %s, voter: %s", vote_req->leader_cand_name_.c_str(), host_node_->GetNodeName().c_str()));
    } else {
      resp = std::unique_ptr<Message>(
          new VoteResponseMessage(
            "you are older than me", 
            vote_req->leader_cand_name_, 
            host_node_->GetNodeName(),
            vote_req->cur_term_, 
            vote_req->cur_index_, 
            false)
          );
      Logger::Debug(Utils::StringFormat("vote denied due to old logs, cand: %s, voter: %s", vote_req->leader_cand_name_.c_str(), host_node_->GetNodeName().c_str()));
    }
  }
  cb(resp.get());
  return Status::OK();
}

raft::Status raft::CandidateHandler::StartElection() {
  if (is_in_election_) {
    return Status::Error("one election is in process");
  }
  auto node_list = host_node_->GetNodeList();

  is_in_election_ = true;
  // TODO: handle cluster change during election
  for (auto kv : node_list) {
    vote_counter_[kv.first] = 0;
  }
  vote_counter_[host_node_->GetNodeName()] = 1;
  vote_for_me_cnt_ = 1;     // vote for myself
  vote_for_other_cnt_ = 1;  // vote for myself

  LogEntryPos latest_pos = log_manager_->GetCurLogEntryPos();
  LogEntryPos new_term_pos = latest_pos.IncreTerm();
  Logger::Debug(Utils::StringFormat("start a new election, term: %d, node: %s", new_term_pos.term_, host_node_->GetNodeName().c_str()));

  // TODO: add retry timout for each rpc
  for (auto e : node_list) {
    if (e.first == host_node_->GetNodeName()) {
      continue;
    }
    VoteRequestMessage req("vote for me", host_node_->GetNodeName(), new_term_pos.term_, new_term_pos.index_);
    // TODO: support sharing same queue for all clients
    //RaftClient c(e.second, receive_queue_);
    RaftClient* c = new RaftClient(e.second);
    c->AsyncAskForVote(req, std::bind(&CandidateHandler::OnVoteResult, this, std::placeholders::_1));
    Logger::Debug(Utils::StringFormat("send vote request to node: %s", e.first.c_str()));
  }
  return Status::OK();
}

void raft::CandidateHandler::OnVoteResult(const Message* resp) {
  const VoteResponseMessage* vote = dynamic_cast<const VoteResponseMessage*>(resp);
  if (!vote) {
    Logger::Err("failed to cast to VoteResponseMessage");
    return;
  }
  // submit to node so that we can handle all msgs sequentially
  const VoteResponseMessage* vote_copy = new VoteResponseMessage(*vote);
  Status s = host_node_->SubmitMessage(vote_copy, [=](const Message*) { delete vote_copy; });
  if (!s.ok()) {
    Logger::Err("failed to submit vote result message");
  }
}

bool raft::CandidateHandler::IsValidVote(const VoteResponseMessage* vote) {
  if (!is_in_election_) {
    return false;
  }
  std::string my_name = host_node_->GetNodeName();
  if (my_name != vote->leader_cand_name_) {
    return false;
  }
  if (vote_counter_.find(my_name) == vote_counter_.end()) {
    return false;
  }
  // get rid of invalid node which is not in current election
  if (vote_counter_.find(vote->voter_name_) == vote_counter_.end()) {
    return false;
  }
  // get rid of duplicate vote requests
  if (vote_counter_[vote->voter_name_] >= 1) {
    return false;
  }
  LogEntryPos latest_pos = log_manager_->GetCurLogEntryPos();
  LogEntryPos elec_pos = latest_pos.IncreTerm();
  if (elec_pos.term_ != vote->vote_term_ || elec_pos.index_ != vote->vote_index_) {
    return false;
  }
  return true;
}

raft::Status raft::CandidateHandler::CountVoteResult(const Message* vote_result) {
  const VoteResponseMessage* vote = dynamic_cast<const VoteResponseMessage*>(vote_result);
  if (!vote) {
    return Status::InvalidArg("failed to cast to VoteResponseMessage");
  }
  Logger::Debug(vote->ToString());
  if (!IsValidVote(vote)) {
    return Status::Error("not a valid vote");
  }
  Logger::Debug(Utils::StringFormat("receive vote result, res: %d, cand: %s, voter: %s, cur vote cnt: %d", vote->grant_or_deny_, vote->leader_cand_name_.c_str(), vote->voter_name_.c_str(), vote_for_me_cnt_));
  vote_counter_[vote->voter_name_]++;
  Status s;
  do {
    if (!vote->grant_or_deny_) {
      s = Status::OK();
    } else {
      vote_for_me_cnt_++;
      // TODO: add timeout for an election that could not get majority
      if (vote_for_me_cnt_ >= host_node_->GetMajority()) {
        s = EndElection(true);
        if (!s.ok()) {
          Logger::Err("failed to end election");
        } else {
          // get majority, return, all remaining msg will be discard due to memship change
          break;
        }
      }
    }
    bool all_votes_received = true;
    for (auto cnt : vote_counter_) {
      if (cnt.second <= 0) {
        all_votes_received = false;
        break;
      }
    }
    if (all_votes_received) {
      Logger::Debug(Utils::StringFormat("didn't get majority, vote cnt: %d", vote_for_me_cnt_));
      // not get majority, start a new election
      EndElection(false);
    }
  } while(0);
  return s;
}

raft::Status raft::CandidateHandler::EndElection(bool elect_succeed) {
  is_in_election_ = false;
  if (elect_succeed) {
    Logger::Debug(Utils::StringFormat("election win, vote cnt: %d", vote_for_me_cnt_));
    log_manager_->StartNewTerm();
    return SwitchMembership(Membership::Leader, "election win");
  } else {
    Logger::Debug(Utils::StringFormat("election lose, vote cnt: %d", vote_for_me_cnt_));
    return ResetTimerForElection();
  }
}

raft::Status raft::CandidateHandler::AddReplicateLog(const Message* req, ResponseCBFunc cb) {
  Status s = SwitchMembership(Membership::Follower, "someone became leader");
  if (!s.ok()) {
    return s;
  }
  s = host_node_->SubmitMessage(req, [=](const Message* resp) { cb(resp); });
  return s;
}
