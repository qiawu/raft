
#ifndef RAFT_HANDLER_H
#define RAFT_HANDLER_H

#include <memory>

#include "replicate_log_manager.h"
#include "utils/status.h"
#include "network/message.h"
#include "network/call_data.h"

namespace raft {
  class RaftNode;

  class RaftHandler {
    public:
      static const uint32_t ElectionMillSecTimeOutMin = 150;
      static const uint32_t ElectionMillSecTimeOutMax = 300;

      RaftHandler(RaftNode* node, ReplicateLogManager* log_manager, Membership type);
      virtual ~RaftHandler();
      virtual Status Init() = 0;
      Status Handle(const Message* req, ResponseCBFunc cb);
      uint32_t GenerateRandomElectionTimeout();
      
      Membership GetType() { return type_; }

    protected:
      virtual Status ProcessMessage(const Message* req, ResponseCBFunc cb) = 0;
      Status SwitchMembership(Membership target, const std::string& reason);

      RaftNode* host_node_;
      ReplicateLogManager* log_manager_;
      Membership type_;

      bool is_switching_membership_;
  };
}

#endif
