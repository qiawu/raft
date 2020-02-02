
#include "raft_handler.h"

#include "raft_node.h"

raft::RaftHandler::RaftHandler(RaftNode* node, HandlerType type): host_node_(node), type_(type) {
}

raft::RaftHandler::~RaftHandler() {

}

