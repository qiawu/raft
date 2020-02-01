
#include "candidate_handler.h"

raft::Status raft::CandidateHandler::Init() {
  return Status::OK();
}

raft::Status raft::CandidateHandler::HandleMessage() {
  return Status::OK();
}

raft::CandidateHandler::~CandidateHandler() {
}

raft::Status raft::CandidateHandler::AddReplicateLog() {
  return Status::OK();
}
raft::Status raft::CandidateHandler::CommitReplicateLog() {
  return Status::OK();

}
