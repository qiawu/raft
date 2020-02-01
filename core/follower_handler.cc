
#include "follower_handler.h"

raft::Status raft::FollowerHandler::Init() {
  return Status::OK();
}

raft::Status raft::FollowerHandler::HandleMessage() {
  return Status::OK();
}

raft::FollowerHandler::~FollowerHandler() {
}

raft::Status raft::FollowerHandler::AddReplicateLog() {
  return Status::OK();
}
raft::Status raft::FollowerHandler::CommitReplicateLog() {
  return Status::OK();

}
