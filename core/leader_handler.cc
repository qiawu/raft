
#include "leader_handler.h"

#include "raft_node.h"

raft::LeaderHandler::~LeaderHandler() {
}

raft::Status raft::LeaderHandler::Init() {
  return Status::OK();
}

raft::Status raft::LeaderHandler::ProcessMessage(const Message* req, ResponseCBFunc cb) {
  Status s;
  switch (req->type_) {
    case MessageType::ElectionTimeout:
      // ignore
      return s;
    case MessageType::VoteResp:
      // ignore
      return s;
    case MessageType::VoteReq:
      return OnAskForVote(req, cb);
    case MessageType::ReplicateEntryReq:
      return AddReplicateLog(req, cb);
    default:
      return s;
  }
}

raft::Status raft::LeaderHandler::OnAskForVote(const Message* req, ResponseCBFunc cb) {
  Status s;
  const VoteRequestMessage* vote_req = dynamic_cast<const VoteRequestMessage*>(req);
  LogEntryPos caller_cur_pos = LogEntryPos(vote_req->cur_term_, vote_req->cur_index_);
  LogEntryPos cur_pos = log_manager_->GetCurLogEntryPos();
  // TODO: check heatbeat from majority
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

raft::Status raft::LeaderHandler::AddReplicateLog(const Message* req, ResponseCBFunc cb) {
  return Status::OK();
}
raft::Status raft::LeaderHandler::CommitReplicateLog() {
  return Status::OK();

}
