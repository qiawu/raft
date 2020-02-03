
#include "candidate_handler.h"

#include "raft_node.h"
#include "utils/timer.h"
#include "utils/status.h"

raft::Status raft::CandidateHandler::Init() {
  Timer t;
  ElectionTimeoutMessage* msg = new ElectionTimeoutMessage("dummy");
  t.SetTimeout([=](){
      this->host_node_->SubmitMessage(msg, [=](const Message&){ delete msg; });
      }, ElectionMillSecTimeOut);
  return Status::OK();
}

raft::Message raft::CandidateHandler::HandleMessage(const Message& req) {
  switch (req.type_) {
    case MessageType::ElectionTimeout:
      StartElection();
      return GeneralMessage("election started");
    case MessageType::VoteReq:
      return GeneralMessage("election started");
    case MessageType::AppendEntryReq:
      return GeneralMessage("election started");
    default:
      return GeneralMessage("election started");
  }
}

raft::CandidateHandler::~CandidateHandler() {
}

raft::Status raft::CandidateHandler::StartElection() {
  auto node_list = host_node_->GetNodeList();
  for (auto e : node_list) {
    LogEntryPos latest_pos = log_manager_->GetCurLogEntryPos();
    VoteRequestMessage req("vote for me", host_node_->GetNodeName(), latest_pos.term_, latest_pos.index_);
    RaftClient c(e.second);
    c.AsyncAskForVote(req, std::bind(&CandidateHandler::OnVoteResult, this, std::placeholders::_1));
  }
  return Status::OK();
}

void raft::CandidateHandler::OnVoteResult(const Message& msg) {
  
}

raft::Status raft::CandidateHandler::AddReplicateLog() {
  return Status::OK();
}
raft::Status raft::CandidateHandler::CommitReplicateLog() {
  return Status::OK();

}
