
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "raft_server.h"
#include "utils/utils.h"
#include "utils/logger.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;

raft::RaftServer::~RaftServer() {
  if (bg_runner_) {
    Logger::Log(Utils::StringFormat("Shutting down server on %s", addr_.ToString().c_str()));
    bg_runner_->Shutdown();
  }
}

raft::Status raft::RaftServer::GetClientAddress(ServerContext* context, NodeAddress& out_addr) {
  std::string peer_raw = context->peer();
  auto res = Utils::Split(peer_raw, ":");
  if (res.size() != 3) {
    return Status::InvalidArg(Utils::StringFormat("invalid client ip %s", peer_raw.c_str()));
  }

  return NodeAddress::ParseNodeAddress(Utils::StringFormat("%s:%s", res[1].c_str(), res[2].c_str()), out_addr);
}

grpc::Status raft::RaftServer::CallToCluster(ServerContext* context, const raft::ClientRequest* request,
    raft::ClientResponse* reply) {
  std::string prefix("Hello ");
  reply->set_message(prefix + request->message());
  return grpc::Status::OK;
}

grpc::Status raft::RaftServer::AskForVote(ServerContext* context, const raft::VoteRequest* request,
    raft::GeneralResponse* reply) {
  NodeAddress addr;
  Status s = GetClientAddress(context, addr);
  if (!s.ok()) {
    return grpc::Status(grpc::StatusCode::UNKNOWN, "invalid client ip");
  }
  auto msg = new VoteRequestMessage(addr, request->node_name(), request->cur_term(), request->cur_index());
  callback_(msg);
  return grpc::Status::OK;
}

grpc::Status raft::RaftServer::VoteForElection(ServerContext* context, const raft::VoteInfo* vote,
    raft::GeneralResponse* reply) {
  return grpc::Status::OK;
}

grpc::Status raft::RaftServer::ReplicateLogEntry(ServerContext* context, const raft::LogEntry* entry,
    raft::GeneralResponse* reply) {
  return grpc::Status::OK;
}

raft::Status raft::RaftServer::Start() {
  std::string server_address(addr_.ToString());

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(this);
  // Finally assemble the server.
  bg_runner_ = builder.BuildAndStart();
  if (!bg_runner_) {
    return Status::Error(Utils::StringFormat("failed to start raft server on %s", server_address.c_str()));
  }
  Logger::Log(Utils::StringFormat("Server listening on %s", server_address.c_str()));

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  //server->Wait();
  return Status::OK();
}

/*
int main(int argc, char** argv) {
  RunServer();

  return 0;
}
*/
