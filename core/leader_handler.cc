
#include "leader_handler.h"

raft::Status raft::LeaderHandler::Init() {
  return Status::OK();
}

raft::Message raft::LeaderHandler::HandleMessage(const Message& req) {
  return GeneralMessage("");
}

raft::LeaderHandler::~LeaderHandler() {
}

raft::Status raft::LeaderHandler::AddReplicateLog() {
  return Status::OK();
}
raft::Status raft::LeaderHandler::CommitReplicateLog() {
  return Status::OK();

}
