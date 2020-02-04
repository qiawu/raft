
#include "candidate_handler.h"

#include "raft_node.h"
#include "utils/status.h"
#include "utils/logger.h"

raft::CandidateHandler::~CandidateHandler() {
  timer_.Stop();
}

raft::Status raft::CandidateHandler::Init() {
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
  std::unique_ptr<Message> resp;
  // TODO: persist voted info, which should be done before sending back vote result (the sender should handle duplicate votes)
  if (vote_for_other_cnt_ >= 1) {
    resp = std::unique_ptr<Message>(new VoteResponseMessage(
          "I have voted for some", 
          vote_req->node_name_, 
          vote_req->cur_term_, 
          vote_req->cur_index_, 
          false)
        );
  }
  LogEntryPos caller_cur_pos = LogEntryPos(vote_req->cur_term_, vote_req->cur_index_);
  bool is_up_to_date = log_manager_->IsMoreUpdateToDate(caller_cur_pos);
  if (is_up_to_date) {
    vote_for_other_cnt_++;
    resp = std::unique_ptr<Message>(
        new VoteResponseMessage(
          "vote granted", 
          vote_req->node_name_, 
          vote_req->cur_term_, 
          vote_req->cur_index_, 
          true)
        );
  } else {
    resp = std::unique_ptr<Message>(
        new VoteResponseMessage(
          "you are older than me", 
          vote_req->node_name_, 
          vote_req->cur_term_, 
          vote_req->cur_index_, 
          false)
        );
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

  // TODO: add retry timout for each rpc
  for (auto e : node_list) {
    LogEntryPos latest_pos = log_manager_->GetCurLogEntryPos();
    LogEntryPos new_term_pos = latest_pos.IncreTerm();
    VoteRequestMessage req("vote for me", host_node_->GetNodeName(), new_term_pos.term_, new_term_pos.index_);
    RaftClient c(e.second);
    c.AsyncAskForVote(req, std::bind(&CandidateHandler::OnVoteResult, this, std::placeholders::_1));
  }
  return Status::OK();
}

void raft::CandidateHandler::OnVoteResult(const Message* resp) {
  const VoteResponseMessage* vote = dynamic_cast<const VoteResponseMessage*>(resp);
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
  if (my_name != vote->vote_name_) {
    return false;
  }
  if (vote_counter_.find(my_name) == vote_counter_.end()) {
    return false;
  }
  // get rid of invalid node which is not in current election
  if (vote_counter_.find(vote->vote_name_) == vote_counter_.end()) {
    return false;
  }
  // get rid of duplicate vote requests
  if (vote_counter_[vote->vote_name_] >= 1) {
    return false;
  }
  LogEntryPos latest_pos = log_manager_->GetCurLogEntryPos();
  if (latest_pos.term_ != vote->vote_term_ || latest_pos.index_ != vote->vote_index_) {
    return false;
  }
  return true;
}

raft::Status raft::CandidateHandler::CountVoteResult(const Message* vote_result) {
  const VoteResponseMessage* vote = dynamic_cast<const VoteResponseMessage*>(vote_result);
  if (!IsValidVote(vote)) {
    return Status::Error("not a valid vote");
  }
  if (vote->grant_or_deny_) {
    vote_for_me_cnt_++;
    vote_counter_[vote->vote_name_]++;
  }
  Status s;
  // TODO: add timeout for an election that could not get majority
  if (vote_for_me_cnt_ >= host_node_->GetMajority()) {
    LogEntryPos latest_pos = log_manager_->GetCurLogEntryPos();
    LogEntryPos new_term_pos = latest_pos.IncreTerm();
    s = EndElection(new_term_pos);
    if (!s.ok()) {
      Logger::Err("failed to end election");
    }
  }
  return s;
}

raft::Status raft::CandidateHandler::EndElection(LogEntryPos new_term_pos) {
  is_in_election_ = false;
  return SwitchMembership(Membership::Leader, "election win");
}

raft::Status raft::CandidateHandler::AddReplicateLog(const Message* req, ResponseCBFunc cb) {
  Status s = SwitchMembership(Membership::Follower, "someone became leader");
  if (!s.ok()) {
    return s;
  }
  s = host_node_->SubmitMessage(req, [=](const Message* resp) { cb(resp); });
  return s;
}
