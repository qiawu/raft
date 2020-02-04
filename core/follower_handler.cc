
#include "follower_handler.h"

#include "raft_node.h"

raft::FollowerHandler::~FollowerHandler() {
  timer_.Stop();
}

raft::Status raft::FollowerHandler::Init() {
  ElectionTimeoutMessage* msg = new ElectionTimeoutMessage("dummy");
  timer_.SetInterval([=](){
      this->host_node_->SubmitMessage(msg, [=](const Message*){ delete msg; });
      }, GenerateRandomElectionTimeout());
  return Status::OK();
}

raft::Status raft::FollowerHandler::ProcessMessage(const Message* req, ResponseCBFunc cb) {
  Status s;
  switch (req->type_) {
    case MessageType::ElectionTimeout:
      return StartElection();
    case MessageType::VoteReq:
      return OnAskForVote(req, cb);
    case MessageType::ReplicateEntryReq:
      return AddReplicateLog(req, cb);
    default:
      return s;
  }
}

raft::Status raft::FollowerHandler::OnAskForVote(const Message* req, ResponseCBFunc cb) {
  Status s;
  const VoteRequestMessage* vote_req = dynamic_cast<const VoteRequestMessage*>(req);
  LogEntryPos caller_cur_pos = LogEntryPos(vote_req->cur_term_, vote_req->cur_index_);
  LogEntryPos cur_pos = log_manager_->GetCurLogEntryPos();
  // TODO: check leader heatbeart within timeout before we decide to vote
  if (caller_cur_pos.term_ > cur_pos.term_) {
    s = SwitchMembership(Membership::Candidate, "someone start a new election");
    if (!s.ok()) {
      return s;
    } else {
      s = host_node_->SubmitMessage(req, cb);
    }
  } else {
    std::unique_ptr<Message> resp = std::unique_ptr<Message>(
        new VoteResponseMessage(
          "you have lower term", 
          vote_req->node_name_, 
          vote_req->cur_term_, 
          vote_req->cur_index_, 
          false)
        );
    cb(resp.get());
  }
  return s;
}

raft::Status raft::FollowerHandler::StartElection() {
    Status s = SwitchMembership(Membership::Candidate, "start a new election due to timeout");
    if (!s.ok()) {
      return s;
    }
    ElectionTimeoutMessage* msg = new ElectionTimeoutMessage("dummy");
    return host_node_->SubmitMessage(msg, [=](const Message* ){ delete msg; });
}

raft::Status raft::FollowerHandler::AddReplicateLog(const Message* req, ResponseCBFunc cb) {
  return Status::OK();
}
raft::Status raft::FollowerHandler::CommitReplicateLog() {
  return Status::OK();

}
