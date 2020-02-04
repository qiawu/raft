
#include "raft_handler.h"

#include <random>

#include "raft_node.h"

raft::RaftHandler::RaftHandler(
    RaftNode* node, 
    ReplicateLogManager* log_manager, 
    Membership type): 
  host_node_(node), 
  log_manager_(log_manager), 
  type_(type), 
  is_switching_membership_(false) {
}

raft::RaftHandler::~RaftHandler() {

}

uint32_t raft::RaftHandler::GenerateRandomElectionTimeout() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(ElectionMillSecTimeOutMin, ElectionMillSecTimeOutMax);
  return dis(gen);
}

raft::Status raft::RaftHandler::Handle(const Message* req, ResponseCBFunc cb) {
  if (is_switching_membership_) {
    // discard election timeout message if membership is switched
    if (req->type_ == MessageType::ElectionTimeout) {
      return Status::OK();
    }
    // resubmit the message so that it can be processed after membership is changed
    host_node_->SubmitMessage(req, cb);
    return Status::OK();
  }
  return ProcessMessage(req, cb);
}

raft::Status raft::RaftHandler::SwitchMembership(Membership target, const std::string& reason) {
  if (type_ == target) {
    return Status::OK();
  }
  auto req = new MembershipSwitchMessage(reason, target);
  Status s = host_node_->SubmitMessage(req, [=](const Message*) { delete req; });
  if (!s.ok()) {
    return s;
  }
  is_switching_membership_ = true;
  return s;
}
