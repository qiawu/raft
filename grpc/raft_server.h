
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
    grpc::Status CallToCluster(ServerContext* context, const raft::ClientRequest* request,
        raft::ClientResponse* reply) override;
    grpc::Status AskForVote(ServerContext* context, const raft::VoteRequest* request,
        raft::GeneralResponse* reply) override;
    grpc::Status VoteForElection(ServerContext* context, const raft::VoteInfo* vote,
        raft::GeneralResponse* reply) override;
    grpc::Status ReplicateLogEntry(ServerContext* context, const raft::LogEntry* entry,
        raft::GeneralResponse* reply) override;
  };

}

#endif
