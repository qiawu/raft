
#include "replicate_log_manager.h"

raft::Status raft::ReplicateLogManager::Init() {
  return Status::OK();

}

raft::Status raft::ReplicateLogManager::StartNewTerm() {
  cur_ = cur_.IncreTerm();
  return Status::OK();
}

raft::Status raft::ReplicateLogManager::AddLog() {
  return Status::OK();

}
raft::Status raft::ReplicateLogManager::CommitLog() {
  return Status::OK();

}

bool raft::ReplicateLogManager::IsMoreUpdateToDate(LogEntryPos new_pos) {
  return true;

}

raft::LogEntryPos raft::ReplicateLogManager::GetCurLogEntryPos() {
  return cur_;

}
raft::LogEntryPos raft::ReplicateLogManager::GetPrevLogEntryPos() {
  return cur_;

}
raft::LogEntryPos raft::ReplicateLogManager::GetLastCommittedLogEntryPos() {
  return cur_;

}

raft::Status raft::ReplicateLogManager::Recover() {
  return Status::OK();

}
raft::Status raft::ReplicateLogManager::PersistLog() {
  return Status::OK();

}
