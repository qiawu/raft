/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>

#include "raft_client.h"

#include "utils/logger.h"
#include "utils/utils.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using raft::ClientRequest;
using raft::ClientResponse;
using raft::RaftService;

raft::RaftClientQueue::RaftClientQueue() {
}

raft::RaftClientQueue::~RaftClientQueue() {
  cq_.Shutdown();
}

void raft::AsyncClientCall::ResetPrevContext() {
  context_ = std::unique_ptr<grpc::ClientContext>(new grpc::ClientContext());
  status_ = std::unique_ptr<grpc::Status>(new grpc::Status());
}

void raft::AsyncClusterClientCall::OnResponse() {
  std::unique_ptr<ClientResponseMessage> resp(new ClientResponseMessage(reply_));
  cb_(resp.get());
}

void raft::AsyncClusterClientCall::InvokeCall(std::unique_ptr<RaftService::Stub>& stub, grpc::CompletionQueue& cq) {
  auto deadline = std::chrono::system_clock::now() +
    std::chrono::milliseconds(timeout_mill_sec_);
  context_->set_deadline(deadline);

  auto response_reader = stub->PrepareAsyncCallToCluster(context_.get(), req_, &cq);
  response_reader->StartCall();
  response_reader->Finish(&reply_, status_.get(), (void*)this);
}

void raft::AsyncVoteClientCall::OnResponse() {
  std::unique_ptr<VoteResponseMessage> resp(new VoteResponseMessage(reply_));
  cb_(resp.get());
}

void raft::AsyncVoteClientCall::InvokeCall(std::unique_ptr<RaftService::Stub>& stub, grpc::CompletionQueue& cq) {
  auto deadline = std::chrono::system_clock::now() +
    std::chrono::milliseconds(timeout_mill_sec_);
  context_->set_deadline(deadline);

  auto response_reader = stub->PrepareAsyncAskForVote(context_.get(), req_, &cq);
  response_reader->StartCall();
  response_reader->Finish(&reply_, status_.get(), (void*)this);
}

void raft::AsyncReplicateClientCall::OnResponse() {
  std::unique_ptr<ReplicateResponseMessage> resp(new ReplicateResponseMessage(reply_));
  cb_(resp.get());
}

void raft::AsyncReplicateClientCall::InvokeCall(std::unique_ptr<RaftService::Stub>& stub, grpc::CompletionQueue& cq) {
  auto deadline = std::chrono::system_clock::now() +
    std::chrono::milliseconds(timeout_mill_sec_);
  context_->set_deadline(deadline);

  auto response_reader = stub->PrepareAsyncReplicateLogEntry(context_.get(), req_, &cq);
  response_reader->StartCall();
  response_reader->Finish(&reply_, status_.get(), (void*)this);
}

raft::RaftClient::RaftClient(const NodeAddress& server_addr, uint32_t timeout_mill_sec, uint32_t max_retry_times): 
  server_addr_(server_addr),
  stub_(RaftService::NewStub(grpc::CreateChannel(server_addr.ToString(), grpc::InsecureChannelCredentials()))),
  timeout_mill_sec_(timeout_mill_sec),
  max_retry_times_(max_retry_times) {
    bg_runner_ = std::thread(&RaftClient::AsyncCompleteRpc, this);
  }

/*
raft::RaftClient::RaftClient(
    const NodeAddress& server_addr, 
    std::shared_ptr<RaftClientQueue> custom_queue, 
    uint32_t timeout_mill_sec,
    uint32_t max_retry_times): 
  server_addr_(server_addr),
  stub_(RaftService::NewStub(grpc::CreateChannel(server_addr.ToString(), grpc::InsecureChannelCredentials()))),
  queue_(custom_queue),
  timeout_mill_sec_(timeout_mill_sec),
  max_retry_times_(max_retry_times) {
  }
  */

raft::RaftClient::~RaftClient() {
  cq_.Shutdown();
  bg_runner_.join();
}

void raft::RaftClient::AsyncCallToCluster(const ClientRequestMessage& req, ResponseCBFunc cb) {
  AsyncClusterClientCall* call = new AsyncClusterClientCall(req, cb, timeout_mill_sec_, max_retry_times_);
  call->InvokeCall(stub_, cq_);
}

void raft::RaftClient::AsyncAskForVote(const VoteRequestMessage& req, ResponseCBFunc cb) {
  AsyncVoteClientCall* call = new AsyncVoteClientCall(req, cb, timeout_mill_sec_, max_retry_times_);
  call->InvokeCall(stub_, cq_);
}

void raft::RaftClient::AsyncReplicateLogEntry(const ReplicateRequestMessage& req, ResponseCBFunc cb) {
  AsyncReplicateClientCall* call = new AsyncReplicateClientCall(req, cb, timeout_mill_sec_, max_retry_times_);
  call->InvokeCall(stub_, cq_);
}

// Loop while listening for completed responses.
// Prints out the response from the server.
void raft::RaftClient::AsyncCompleteRpc() {
  void* got_tag;
  bool ok = false;
  const auto pause_time_mill_sec = std::chrono::milliseconds(1000);

  // Block until the next result is available in the completion queue "cq".
  while (cq_.Next(&got_tag, &ok)) {
    // The tag in this example is the memory location of the call object
    AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);

    // Verify that the request was completed successfully. Note that "ok"
    // corresponds solely to the request for updates introduced by Finish().
    GPR_ASSERT(ok);

    if (call->status_->ok()) {
      call->OnResponse();
    } else {
      if (call->left_retry_times_ > 0) {
        std::this_thread::sleep_for(pause_time_mill_sec);
        call->left_retry_times_--;
        Logger::Debug(Utils::StringFormat("request retrying remaining %d times, %d timeout", call->left_retry_times_, call->timeout_mill_sec_));
        call->ResetPrevContext();
        call->InvokeCall(stub_, cq_);
        continue;
      } else {
        Logger::Debug(Utils::StringFormat("request failed after retrying %d times", max_retry_times_));
      }
    }

    // Once we're complete, deallocate the call object.
    delete call;
  }
  Logger::Debug(Utils::StringFormat("client binding to %s is exited", server_addr_.ToString().c_str()));
}

