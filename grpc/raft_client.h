
#ifndef RAFT_CLIENT_H
#define RAFT_CLIENT_H

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "grpc/raft.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;

namespace raft {
  class RaftClient {
   public:
    RaftClient(std::shared_ptr<Channel> channel);

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::string SendRequest(const std::string& user);

   private:
    std::unique_ptr<RaftService::Stub> stub_;
  };

}

#endif
