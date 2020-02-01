
#include "raft_node.h"

raft::RaftNode::RaftNode(): handler_(nullptr) {

}

raft::Status raft::RaftNode::Initialize(const std::string& conf_path) {
  Status s = LoadConf(conf_path);
  if (!s.ok()) {
    return s;
  }
  return s;
}

raft::Status raft::RaftNode::LoadConf(const std::string& conf_path) {
  return Status::OK();
}

raft::Status raft::RaftNode::OnMessage(const Message& msg) {
  return Status::OK();

}

raft::Status raft::RaftNode::SendMessage(const NodeIdentify& receiver, uint32_t sec_timeout, uint32_t retry_times) {
  return Status::OK();

}

raft::Status raft::RaftNode::SwitchMemship(HandlerType src, HandlerType tgt) {
  return Status::OK();

}
