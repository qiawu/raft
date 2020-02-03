
#ifndef RAFT_CLIENT_H
#define RAFT_CLIENT_H

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "network/raft.grpc.pb.h"

namespace raft {
  class RaftClient {
   public:
    RaftClient(std::shared_ptr<grpc::Channel> channel);

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    void AsyncSendRequest(const std::string& user);

    void AsyncCompleteRpc();

  private:
    // struct for keeping state and data information
    struct AsyncClientCall {
        // Container for the data we expect from the server.
        ClientResponse reply_;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        grpc::ClientContext context_;

        // Storage for the status of the RPC upon completion.
        grpc::Status status_;

        std::unique_ptr<grpc::ClientAsyncResponseReader<ClientResponse>> response_reader_;
    };

    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    std::unique_ptr<RaftService::Stub> stub_;

    // The producer-consumer queue we use to communicate asynchronously with the
    // gRPC runtime.
    grpc::CompletionQueue cq_;
  };

}

#endif
