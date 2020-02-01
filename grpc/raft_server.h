
#ifndef RAFT_SERVER_H
#define RAFT_SERVER_H

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "grpc/raft.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;

namespace raft {
  // Logic and data behind the server's behavior.
  class RaftServiceImpl final : public raft::RaftService::Service {
    grpc::Status HandleClientRequest(ServerContext* context, const raft::ClientRequest* request,
                    raft::ClientResponse* reply) override;
    grpc::Status HandleVote(ServerContext* context, const raft::VoteRequest* request,
                    raft::VoteResponse* reply) override;
    grpc::Status HandleReplication(ServerContext* context, const raft::ReplicateRequest* request,
                    raft::ReplicateResponse* reply) override;
  };

}

#endif
