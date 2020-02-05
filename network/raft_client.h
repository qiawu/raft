
#ifndef RAFT_CLIENT_H
#define RAFT_CLIENT_H

#include <iostream>
#include <memory>
#include <string>
#include <limits>
#include <thread>

#include <grpcpp/grpcpp.h>

#include "network/raft.grpc.pb.h"
#include "call_data.h"
#include "message.h"


namespace raft {

  class RaftClient;

  struct AsyncClientCall {
    AsyncClientCall(
        ResponseCBFunc cb, 
        uint32_t timeout_mill_sec, 
        uint32_t left_retry_times): 
      context_(new grpc::ClientContext()),
      status_(new grpc::Status()),
      cb_(cb), 
      timeout_mill_sec_(timeout_mill_sec), 
      left_retry_times_(left_retry_times) {}
    virtual ~AsyncClientCall() {}
    virtual void InvokeCall(std::unique_ptr<RaftService::Stub>& stub, grpc::CompletionQueue& cq) = 0;
    virtual void OnResponse() = 0;

    void ResetPrevContext();

    std::unique_ptr<grpc::ClientContext> context_;
    std::unique_ptr<grpc::Status> status_;
    ResponseCBFunc cb_;
    uint32_t timeout_mill_sec_;
    uint32_t left_retry_times_;
  };

  struct AsyncClusterClientCall: public AsyncClientCall {
    AsyncClusterClientCall(
        const ClientRequestMessage& req, 
        ResponseCBFunc cb, 
        uint32_t timeout_mill_sec, 
        uint32_t left_retry_times): 
      AsyncClientCall(cb, timeout_mill_sec, left_retry_times), 
      req_(req.ToProtoType()) {}
    void InvokeCall(std::unique_ptr<RaftService::Stub>& stub, grpc::CompletionQueue& cq) override;
    void OnResponse() override;
    ClientRequest req_;
    ClientResponse reply_;
  };
  struct AsyncVoteClientCall: public AsyncClientCall {
    AsyncVoteClientCall(
        const VoteRequestMessage& req, 
        ResponseCBFunc cb, 
        uint32_t timeout_mill_sec,
        uint32_t left_retry_times): 
      AsyncClientCall(cb, timeout_mill_sec, left_retry_times), 
      req_(req.ToProtoType()) {}
    void InvokeCall(std::unique_ptr<RaftService::Stub>& stub, grpc::CompletionQueue& cq) override;
    void OnResponse() override;

    VoteRequest req_;
    VoteResponse reply_;
  };
  struct AsyncReplicateClientCall: public AsyncClientCall {
    AsyncReplicateClientCall(
        const ReplicateRequestMessage& req, 
        ResponseCBFunc cb, 
        uint32_t timeout_mill_sec,
        uint32_t left_retry_times): 
      AsyncClientCall(cb, timeout_mill_sec, left_retry_times),
      req_(req.ToProtoType()) {}
    void InvokeCall(std::unique_ptr<RaftService::Stub>& stub, grpc::CompletionQueue& cq) override;
    void OnResponse() override;

    ReplicateRequest req_;
    ReplicateResponse reply_;
  };


  class RaftClientQueue {
    public:
      RaftClientQueue();
      ~RaftClientQueue();

    private:
      // The producer-consumer queue we use to communicate asynchronously with the
      // gRPC runtime.
      // make it static so that all connections can share the same queue
      grpc::CompletionQueue cq_;

    friend RaftClient;
  };

  class RaftClient {
   public:
     static const uint32_t RaftClientDefaultMillSecTimeout = 50;
     static const uint32_t RaftClientDefaultMaxRetryTimes = 500;

    RaftClient(
        const NodeAddress& server_addr, 
        uint32_t timeout_mill_sec = RaftClientDefaultMillSecTimeout,
        uint32_t max_retry_times = RaftClientDefaultMaxRetryTimes);
    // TODO: support sharing same queue for all clients
    /*
    RaftClient(
        const NodeAddress& server_addr, 
        std::shared_ptr<RaftClientQueue> custom_queue, 
        uint32_t timeout_mill_sec = RaftClientDefaultMillSecTimeout,
        uint32_t max_retry_times = RaftClientDefaultMaxRetryTimes);
        */
    ~RaftClient();

    void AsyncCallToCluster(const ClientRequestMessage& req, ResponseCBFunc cb);
    void AsyncAskForVote(const VoteRequestMessage& req, ResponseCBFunc cb);
    void AsyncReplicateLogEntry(const ReplicateRequestMessage& req, ResponseCBFunc cb);

    void AsyncCompleteRpc();

  private:
    std::thread bg_runner_;
    NodeAddress server_addr_;
    // Out of the passed in Channel comes the stub, stored here, our view of the
    // server's exposed services.
    std::unique_ptr<RaftService::Stub> stub_;

    grpc::CompletionQueue cq_;
    // TODO: support sharing same queue for all clients
    //RaftClientQueue queue_;

    uint32_t timeout_mill_sec_;
    uint32_t max_retry_times_;

    friend AsyncClientCall;
    friend AsyncClusterClientCall;
    friend AsyncVoteClientCall;
    friend AsyncReplicateClientCall;
  };

}

#endif
