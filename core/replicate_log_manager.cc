
#include "replicate_log_manager.h"

raft::ReplicateLogManager::ReplicateLogManager() {

}

raft::Status raft::ReplicateLogManager::Init() {
  return Status::OK();

}

raft::Status raft::ReplicateLogManager::AddLog() {
  return Status::OK();

}
raft::Status raft::ReplicateLogManager::CommitLog() {
  return Status::OK();

}

bool raft::ReplicateLogManager::IsMoreUpdateToDate() {
  return true;

}

raft::LogEntryPos raft::ReplicateLogManager::GetCurLogEntryPos() {
  return LogEntryPos();

}
raft::LogEntryPos raft::ReplicateLogManager::GetPrevLogEntryPos() {
  return LogEntryPos();

}
raft::LogEntryPos raft::ReplicateLogManager::GetLastCommittedLogEntryPos() {
  return LogEntryPos();

}

raft::Status raft::ReplicateLogManager::Recover() {
  return Status::OK();

}
raft::Status raft::ReplicateLogManager::PersistLog() {
  return Status::OK();

}
