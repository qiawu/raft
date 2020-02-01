
#include "leader_handler.h"

raft::Status raft::LeaderHandler::Init() {
  return Status::OK();
}

raft::Status raft::LeaderHandler::HandleMessage() {
  return Status::OK();
}

raft::LeaderHandler::~LeaderHandler() {
}

raft::Status raft::LeaderHandler::AddReplicateLog() {
  return Status::OK();
}
raft::Status raft::LeaderHandler::CommitReplicateLog() {
  return Status::OK();

}
