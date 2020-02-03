
#include "call_data.h"

#include "utils/logger.h"

raft::ClientCall::ClientCall(CallData* data) : RemoteCall(data), responder_(&ctx_), status_(REQUEST) {
  // As part of the initial CREATE state, we *request* that the system
  // start processing SayHello requests. In this request, "this" acts are
  // the tag uniquely identifying the request (so that different CallData
  // instances can serve different requests concurrently), in this case
  // the memory address of this CallData instance.
  data_->service_->RequestCallToCluster(&ctx_, &request_, &responder_, data_->cq_, data_->cq_,
      this);
}

void raft::ClientCall::Proceed(bool ok) {
  switch (status_) {
    case REQUEST:
      new ClientCall(data_);
      if (!ok) {
        Logger::Log("Get in REQUEST was not ok. Finishing.");
        responder_.FinishWithError(grpc::Status::CANCELLED, this);
        status_ = FINISH;
        break;
      }
      // The actual processing.
      reply_.set_message("Hello " + request_.message());
      // And we are done! Let the gRPC runtime know we've finished, using the
      // memory address of this instance as the uniquely identifying tag for
      // the event.
      status_ = FINISH;
      responder_.Finish(reply_, grpc::Status::OK, this);
      break;

    case FINISH:
      if (!ok)
        Logger::Log("Get RPC finished unexpectedly");
      delete this;
      break;
  }
}

raft::ElectionCall::ElectionCall(CallData* data) : RemoteCall(data), responder_(&ctx_), status_(REQUEST) {
  // As part of the initial CREATE state, we *request* that the system
  // start processing SayHello requests. In this request, "this" acts are
  // the tag uniquely identifying the request (so that different CallData
  // instances can serve different requests concurrently), in this case
  // the memory address of this CallData instance.
  data_->service_->RequestAskForVote(&ctx_, &request_, &responder_, data_->cq_, data_->cq_,
      this);
}

void raft::ElectionCall::Proceed(bool ok) {
  switch (status_) {
    case REQUEST:
      new ElectionCall(data_);
      if (!ok) {
        Logger::Log("Get in REQUEST was not ok. Finishing.");
        responder_.FinishWithError(grpc::Status::CANCELLED, this);
        status_ = FINISH;
        break;
      }
      // And we are done! Let the gRPC runtime know we've finished, using the
      // memory address of this instance as the uniquely identifying tag for
      // the event.
      status_ = FINISH;
      responder_.Finish(reply_, grpc::Status::OK, this);
      break;

    case FINISH:
      if (!ok)
        Logger::Log("Get RPC finished unexpectedly");
      delete this;
      break;
  }
}

raft::ReplicateCall::ReplicateCall(CallData* data) : RemoteCall(data), responder_(&ctx_), status_(REQUEST) {
  // As part of the initial CREATE state, we *request* that the system
  // start processing SayHello requests. In this request, "this" acts are
  // the tag uniquely identifying the request (so that different CallData
  // instances can serve different requests concurrently), in this case
  // the memory address of this CallData instance.
  data_->service_->RequestReplicateLogEntry(&ctx_, &request_, &responder_, data_->cq_, data_->cq_,
      this);
}

void raft::ReplicateCall::Proceed(bool ok) {
  switch (status_) {
    case REQUEST:
      new ReplicateCall(data_);
      if (!ok) {
        Logger::Log("Get in REQUEST was not ok. Finishing.");
        responder_.FinishWithError(grpc::Status::CANCELLED, this);
        status_ = FINISH;
        break;
      }
      // And we are done! Let the gRPC runtime know we've finished, using the
      // memory address of this instance as the uniquely identifying tag for
      // the event.
      status_ = FINISH;
      responder_.Finish(reply_, grpc::Status::OK, this);
      break;

    case FINISH:
      if (!ok)
        Logger::Log("Get RPC finished unexpectedly");
      delete this;
      break;
  }
}
