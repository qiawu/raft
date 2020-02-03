#ifndef CALL_DATA_H
#define CALL_DATA_H 

#include <grpcpp/grpcpp.h>

#include "network/raft.grpc.pb.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;

namespace raft {


  enum CallStatus { REQUEST, FINISH };

  struct CallData {
    CallData(RaftService::AsyncService* service, grpc::ServerCompletionQueue* cq) : service_(service), cq_(cq) {}
    RaftService::AsyncService* service_;
    grpc::ServerCompletionQueue* cq_;
  };

  // Base class used to cast the void* tags we get from
  // the completion queue and call Proceed() on them.
  class RemoteCall {
    public:
      RemoteCall(CallData* data): data_(data) {}
      virtual void Proceed(bool ok) = 0;

    protected:
      CallData* data_;
      // Context for the rpc, allowing to tweak aspects of it such as the use
      // of compression, authentication, as well as to send metadata back to the
      // client.
      grpc::ServerContext ctx_;
  };

  class ClientCall : public RemoteCall {
    public:
      explicit ClientCall(CallData* data);
      void Proceed(bool ok) override;

    private:
      grpc::Status CallToCluster(ServerContext* context, const raft::ClientRequest* request, raft::ClientResponse* reply);
      
      CallStatus status_;
      ClientRequest request_;
      ClientResponse reply_;

      grpc::ServerAsyncResponseWriter<ClientResponse> responder_;

  };

  class ElectionCall : public RemoteCall {
    public:
      explicit ElectionCall(CallData* data);
      void Proceed(bool ok) override;

    private:
      grpc::Status AskForVote();
      
      CallStatus status_;
      VoteRequest request_;
      VoteResponse reply_;

      grpc::ServerAsyncResponseWriter<VoteResponse> responder_;

  };

  class ReplicateCall : public RemoteCall {
    public:
      explicit ReplicateCall(CallData* data);
      void Proceed(bool ok) override;

    private:
      grpc::Status ReplicateLogEntry();
      
      CallStatus status_;
      ReplicateRequest request_;
      ReplicateResponse reply_;

      grpc::ServerAsyncResponseWriter<ReplicateResponse> responder_;

  };
}

#endif /* CALL_DATA_H */
