
#include "raft_node.h"

#include <functional>

#include "utils/configuration.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include "candidate_handler.h"
#include "follower_handler.h"
#include "leader_handler.h"

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
  s = InitReplicateLog();
  if (!s.ok()) {
    return s;
  }
  s = InitHandler();
  if (!s.ok()) {
    return s;
  }
  s = StartMessageProcessor();
  if (!s.ok()) {
    return s;
  }
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

raft::Status raft::RaftNode::InitReplicateLog() {
  return log_manager_.Init();
}

raft::Status raft::RaftNode::InitHandler() {
  handler_ = std::unique_ptr<RaftHandler>(new CandidateHandler(this, &log_manager_));
  return handler_->Init();
}

raft::Status raft::RaftNode::StartServer() {
  raft_server_ = std::unique_ptr<RaftServer>(new RaftServer(node_list_[local_name_], std::bind(&RaftNode::SubmitMessage, this, std::placeholders::_1, std::placeholders::_2)));
  return raft_server_->Start();
}

raft::Status raft::RaftNode::StartMessageProcessor() {
  bg_msg_processor_ = std::thread(&RaftNode::ProcessMessages, this);
  return Status::OK();
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
      Status s = HandleMessage(item.msg_, item.cb_);
      if (!s.ok()) {
        Logger::Err(Utils::StringFormat("failed to handle message: %s", s.ToString().c_str()));
      }
    }
  }
}

raft::Status raft::RaftNode::SubmitMessage(const Message* msg, ResponseCBFunc cb) {
  // TODO: set queue limit
  msg_queue_.Push(QueueItem(msg, cb));
  return Status::OK();
}

raft::Status raft::RaftNode::HandleMessage(const Message* req, ResponseCBFunc cb) {
  if (req->type_ == MessageType::MembershipSwitch) {
    const MembershipSwitchMessage* msg = dynamic_cast<const MembershipSwitchMessage*>(req);
    if (!msg) {
      return Status::InvalidArg("failed to cast to MembershipSwitchMessage");
    }
    Status s;
    switch (msg->target_) {
      case Membership::Leader:
        handler_ = std::unique_ptr<RaftHandler>(new LeaderHandler(this, &log_manager_));
        break;
      case Membership::Follower:
        handler_ = std::unique_ptr<RaftHandler>(new FollowerHandler(this, &log_manager_));
        break;
      case Membership::Candidate:
        handler_ = std::unique_ptr<RaftHandler>(new CandidateHandler(this, &log_manager_));
        break;
      default:
        s = Status::InvalidArg("invalid target handler type");
        break;
    }
    return s;
  }
  return handler_->Handle(req, cb);
}

uint32_t raft::RaftNode::GetMajority() {
  return node_list_.size()/2 + 1;
}

