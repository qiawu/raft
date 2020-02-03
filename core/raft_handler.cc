
#include "raft_handler.h"

#include "raft_node.h"

raft::RaftHandler::RaftHandler(RaftNode* node, ReplicateLogManager* log_manager, HandlerType type): host_node_(node), log_manager_(log_manager), type_(type) {
}

raft::RaftHandler::~RaftHandler() {

}

