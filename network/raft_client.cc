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

grpc::CompletionQueue raft::RaftClient::cq_;

void raft::AsyncClusterClientCall::OnResponse() {
  std::unique_ptr<ClientResponseMessage> resp(new ClientResponseMessage(reply_));
  cb_(resp.get());
}

void raft::AsyncVoteClientCall::OnResponse() {
  std::unique_ptr<VoteResponseMessage> resp(new VoteResponseMessage(reply_));
  cb_(resp.get());
}

void raft::AsyncReplicateClientCall::OnResponse() {
  std::unique_ptr<ReplicateResponseMessage> resp(new ReplicateResponseMessage(reply_));
  cb_(resp.get());
}

raft::RaftClient::RaftClient(const NodeAddress& server_addr)
  : stub_(RaftService::NewStub(grpc::CreateChannel(server_addr.ToString(), grpc::InsecureChannelCredentials()))) {
  }

void raft::RaftClient::AsyncCallToCluster(const ClientRequestMessage& req, ResponseCBFunc cb) {
  ClientRequest request;
  request.set_message(req.msg_);

  AsyncClusterClientCall* call = new AsyncClusterClientCall(cb);
  call->cb_ = cb;
  auto response_reader = stub_->PrepareAsyncCallToCluster(&call->context_, request, &cq_);
  response_reader->StartCall();
  response_reader->Finish(&call->reply_, &call->status_, (void*)call);
}

void raft::RaftClient::AsyncAskForVote(const VoteRequestMessage& req, ResponseCBFunc cb) {
  VoteRequest request;
  request.set_node_name(req.node_name_);
  request.set_cur_term(req.cur_term_);
  request.set_cur_index(req.cur_index_);

  AsyncVoteClientCall* call = new AsyncVoteClientCall(cb);
  call->cb_ = cb;
  auto response_reader = stub_->PrepareAsyncAskForVote(&call->context_, request, &cq_);
  response_reader->StartCall();
  response_reader->Finish(&call->reply_, &call->status_, (void*)call);
}

void raft::RaftClient::AsyncReplicateLogEntry(const ReplicateRequestMessage& req, ResponseCBFunc cb) {
  ReplicateRequest request;

  AsyncReplicateClientCall* call = new AsyncReplicateClientCall(cb);
  auto response_reader = stub_->PrepareAsyncReplicateLogEntry(&call->context_, request, &cq_);
  response_reader->StartCall();
  response_reader->Finish(&call->reply_, &call->status_, (void*)call);
}

// Loop while listening for completed responses.
// Prints out the response from the server.
void raft::RaftClient::AsyncCompleteRpc() {
  void* got_tag;
  bool ok = false;

  // Block until the next result is available in the completion queue "cq".
  while (cq_.Next(&got_tag, &ok)) {
    // The tag in this example is the memory location of the call object
    AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);

    // Verify that the request was completed successfully. Note that "ok"
    // corresponds solely to the request for updates introduced by Finish().
    GPR_ASSERT(ok);

    if (call->status_.ok()) {
      call->OnResponse();
    } else {
      Logger::Err("RPC failed");
    }

    // Once we're complete, deallocate the call object.
    delete call;
  }
}

