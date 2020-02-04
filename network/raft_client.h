
#ifndef RAFT_CLIENT_H
#define RAFT_CLIENT_H

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "network/raft.grpc.pb.h"
#include "call_data.h"
#include "message.h"

namespace raft {
  struct AsyncClientCall {
    AsyncClientCall(ResponseCBFunc cb): cb_(cb) {}
    virtual ~AsyncClientCall() {}
    virtual void OnResponse() = 0;
    grpc::ClientContext context_;
    grpc::Status status_;
    ResponseCBFunc cb_;
  };

  struct AsyncClusterClientCall: public AsyncClientCall {
    AsyncClusterClientCall(ResponseCBFunc cb): AsyncClientCall(cb) {}
    ClientResponse reply_;
    void OnResponse() override;
  };
  struct AsyncVoteClientCall: public AsyncClientCall {
    AsyncVoteClientCall(ResponseCBFunc cb): AsyncClientCall(cb) {}
    VoteResponse reply_;
    void OnResponse() override;
  };
  struct AsyncReplicateClientCall: public AsyncClientCall {
    AsyncReplicateClientCall(ResponseCBFunc cb): AsyncClientCall(cb) {}
    ReplicateResponse reply_;
    void OnResponse() override;
  };

  class RaftClient {
   public:
    RaftClient(const NodeAddress& server_addr);

    void AsyncCallToCluster(const ClientRequestMessage& req, ResponseCBFunc cb);
    void AsyncAskForVote(const VoteRequestMessage& req, ResponseCBFunc cb);
    void AsyncReplicateLogEntry(const ReplicateRequestMessage& req, ResponseCBFunc cb);

    void AsyncCompleteRpc();

  private:
    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    std::unique_ptr<RaftService::Stub> stub_;

    // The producer-consumer queue we use to communicate asynchronously with the
    // gRPC runtime.
    // make it static so that all connections can share the same queue
    static grpc::CompletionQueue cq_;
  };

}

#endif
