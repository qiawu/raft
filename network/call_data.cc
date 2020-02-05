
#include "call_data.h"

#include "utils/logger.h"

raft::ClientCall::ClientCall(CallData* data) : RemoteCall(data), responder_(&ctx_), status_(REQUEST) {
  data_->service_->RequestCallToCluster(&ctx_, &request_, &responder_, data_->cq_, data_->cq_,
      this);
}

void raft::ClientCall::Callback(const Message* reply_wrapper) {
  // And we are done! Let the gRPC runtime know we've finished, using the
  // memory address of this instance as the uniquely identifying tag for
  // the event.
  const ClientResponseMessage* resp = dynamic_cast<const ClientResponseMessage*>(reply_wrapper);
  if (!resp) {
    Logger::Err("failed to cast to ClientResponseMessage");
    return;
  }
  status_ = FINISH;
  responder_.Finish(resp->ToProtoType(), grpc::Status::OK, this);
}

void raft::ClientCall::Proceed(bool ok) {
  switch (status_) {
    case REQUEST:
      new ClientCall(data_);
      if (!ok) {
        Logger::Err("Get in REQUEST was not ok. Finishing.");
        responder_.FinishWithError(grpc::Status::CANCELLED, this);
        status_ = FINISH;
        break;
      }
      // The actual processing.
      req_wrapper_ = new ClientRequestMessage(request_);
      data_->handle_func_(req_wrapper_, std::bind(&ClientCall::Callback, this, std::placeholders::_1));
      break;

    case FINISH:
      if (!ok)
        Logger::Err("Get RPC finished unexpectedly");
      delete this;
      break;
  }
}

raft::ElectionCall::ElectionCall(CallData* data) : RemoteCall(data), responder_(&ctx_), status_(REQUEST) {
  data_->service_->RequestAskForVote(&ctx_, &request_, &responder_, data_->cq_, data_->cq_,
      this);
}

void raft::ElectionCall::Callback(const Message* reply_wrapper) {
  // And we are done! Let the gRPC runtime know we've finished, using the
  // memory address of this instance as the uniquely identifying tag for
  // the event.
  const VoteResponseMessage* resp = dynamic_cast<const VoteResponseMessage*>(reply_wrapper);
  if (!resp) {
    Logger::Err("failed to cast to VoteResponseMessage");
    return;
  }
  status_ = FINISH;
  responder_.Finish(resp->ToProtoType(), grpc::Status::OK, this);
}

void raft::ElectionCall::Proceed(bool ok) {
  switch (status_) {
    case REQUEST:
      new ElectionCall(data_);
      if (!ok) {
        Logger::Err("Get in REQUEST was not ok. Finishing.");
        responder_.FinishWithError(grpc::Status::CANCELLED, this);
        status_ = FINISH;
        break;
      }
      // The actual processing.
      req_wrapper_ = new VoteRequestMessage(request_);
      data_->handle_func_(req_wrapper_, std::bind(&ElectionCall::Callback, this, std::placeholders::_1));
      break;

    case FINISH:
      if (!ok)
        Logger::Err("Get RPC finished unexpectedly");
      delete this;
      break;
  }
}

raft::ReplicateCall::ReplicateCall(CallData* data) : RemoteCall(data), responder_(&ctx_), status_(REQUEST) {
  data_->service_->RequestReplicateLogEntry(&ctx_, &request_, &responder_, data_->cq_, data_->cq_,
      this);
}

void raft::ReplicateCall::Callback(const Message* reply_wrapper) {
  // And we are done! Let the gRPC runtime know we've finished, using the
  // memory address of this instance as the uniquely identifying tag for
  // the event.
  const ReplicateResponseMessage* resp = dynamic_cast<const ReplicateResponseMessage*>(reply_wrapper);
  if (!resp) {
    Logger::Err("failed to cast to ReplicateResponseMessage");
    return;
  }
  status_ = FINISH;
  responder_.Finish(resp->ToProtoType(), grpc::Status::OK, this);
}

void raft::ReplicateCall::Proceed(bool ok) {
  switch (status_) {
    case REQUEST:
      new ReplicateCall(data_);
      if (!ok) {
        Logger::Err("Get in REQUEST was not ok. Finishing.");
        responder_.FinishWithError(grpc::Status::CANCELLED, this);
        status_ = FINISH;
        break;
      }
      // The actual processing.
      req_wrapper_ = new ReplicateRequestMessage(request_);
      data_->handle_func_(req_wrapper_, std::bind(&ReplicateCall::Callback, this, std::placeholders::_1));
      break;

    case FINISH:
      if (!ok)
        Logger::Err("Get RPC finished unexpectedly");
      delete this;
      break;
  }
}
