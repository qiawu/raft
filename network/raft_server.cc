
#include "raft_server.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "call_data.h"
#include "utils/utils.h"
#include "utils/logger.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;

raft::RaftServer::~RaftServer() {
}

raft::Status raft::RaftServer::GetClientAddress(ServerContext* context, NodeAddress& out_addr) {
  std::string peer_raw = context->peer();
  auto res = Utils::Split(peer_raw, ":");
  if (res.size() != 3) {
    return Status::InvalidArg(Utils::StringFormat("invalid client ip %s", peer_raw.c_str()));
  }

  return NodeAddress::ParseNodeAddress(Utils::StringFormat("%s:%s", res[1].c_str(), res[2].c_str()), out_addr);
}

raft::Status raft::RaftServer::Start() {
  std::string server_address(addr_.ToString());

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service_" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *asynchronous* service.
  builder.RegisterService(&async_service_);
  // Get hold of the completion queue used for the asynchronous communication
  // with the gRPC runtime.
  async_cq_ = builder.AddCompletionQueue();
  // Finally assemble the server.
  async_server_ = builder.BuildAndStart();
  std::cout << "Server listening on " << server_address << std::endl;

  if (!async_server_) {
    return Status::Error(Utils::StringFormat("failed to start raft server on %s", server_address.c_str()));
  }
  Logger::Info(Utils::StringFormat("Server listening on %s", server_address.c_str()));

  // Proceed to the server's main loop.
  //bg_handler_ = std::thread(&RaftServer::HandleRpcs, this);
  // block
  HandleRpcs();
  return Status::OK();
}

void raft::RaftServer::HandleRpcs() {
  // Spawn a new CallData instance to serve new clients.
  CallData data(handle_func_, &async_service_, async_cq_.get());
  new ClientCall(&data);
  new ElectionCall(&data);
  new ReplicateCall(&data);
  void* tag;  // uniquely identifies a request.
  bool ok = false;
  while (true) {
    if (is_server_shutting_down_) {
      break;
    }
    // Block waiting to read the next event from the completion queue. The
    // event is uniquely identified by its tag, which in this case is the
    // memory address of a CallData instance.
    // The return value of Next should always be checked. This return value
    // tells us whether there is any kind of event or cq_ is shutting down.
    GPR_ASSERT(async_cq_->Next(&tag, &ok));
    static_cast<RemoteCall*>(tag)->Proceed(ok);
  }
}

raft::Status raft::RaftServer::Shutdown() {
  if (async_cq_) {
    async_cq_->Shutdown();
  }
  if (async_server_) {
    Logger::Info(Utils::StringFormat("Shutting down server on %s", addr_.ToString().c_str()));
    async_server_->Shutdown();
  }
  is_server_shutting_down_ = true;
  //bg_handler_.join();
  return Status::OK();
}
/*
int main(int argc, char** argv) {
  RunServer();

  return 0;
}
*/
