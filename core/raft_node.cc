
#include "raft_node.h"

#include <functional>

#include "utils/configuration.h"

raft::RaftNode::~RaftNode() {
  is_node_shutting_down_ = true;
  bg_msg_processor_.join();
  if (raft_server_) {
    raft_server_->Shutdown();
  }
}

raft::Status raft::RaftNode::Initialize(const std::string& conf_path) {
  Status s = LoadConf(conf_path);
  if (!s.ok()) {
    return s;
  }
  raft_server_ = std::unique_ptr<RaftServer>(new RaftServer(node_list_[local_name_], std::bind(&RaftNode::SubmitMessage, this, std::placeholders::_1, std::placeholders::_2)));
  raft_server_->Start();
  bg_msg_processor_ = std::thread(&RaftNode::ProcessMessages, this);
  return s;
}

raft::Status raft::RaftNode::LoadConf(const std::string& conf_path) {
  Configuration conf(conf_path);
  Status s = conf.Parse();
  if (!s.ok()) {
    return s;
  }
  auto nodes_info = conf.GetPropertiesBySection(Configuration::kNodeListSectionHeader);
  for (auto n : nodes_info) {
    NodeAddress addr;
    s = NodeAddress::ParseNodeAddress(n.second, addr);
    if (!s.ok()) {
      return s;
    }
    node_list_.insert(make_pair(n.first, addr));
  }
  auto local_info = conf.GetPropertiesBySection(Configuration::kLocalSectionHeader);
  auto it = local_info.find(Configuration::kLocalNamePropertyKey);
  if (it != local_info.end() && node_list_.find(it->second) != node_list_.end()) {
    local_name_ = it->second;
    return Status::OK();
  } else {
    return Status::InvalidArg("no local name for node");
  }
}

void raft::RaftNode::ProcessMessages() {
  const int max_wait_time = 1000;
  while(true) {
    if (is_node_shutting_down_) {
      break;
    }
    QueueItem item;
    bool has_val = msg_queue_.TryWaitAndPop(item, max_wait_time);
    if (has_val) {
      Message* req = item.msg_;
      Message resp = HandleMessage(*req);
      item.cb_(&resp);
    }
  }
}

raft::Status raft::RaftNode::SubmitMessage(Message* msg, ResponseCBFunc cb) {
  // TODO: set queue limit
  msg_queue_.Push(QueueItem(msg, cb));
  return Status::OK();
}

raft::Message raft::RaftNode::HandleMessage(const Message& req) {
  return Message("done");
}

raft::Status raft::RaftNode::SendMessage(const NodeIdentify& receiver, uint32_t sec_timeout, uint32_t retry_times) {
  return Status::OK();

}

raft::Status raft::RaftNode::SwitchMemship(HandlerType src, HandlerType tgt) {
  return Status::OK();

}
