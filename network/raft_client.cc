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

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using raft::ClientRequest;
using raft::ClientResponse;
using raft::RaftService;

raft::RaftClient::RaftClient(std::shared_ptr<Channel> channel)
      : stub_(RaftService::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
std::string raft::RaftClient::SendRequest(const std::string& user) {
  // Data we are sending to the server.
  ClientRequest request;
  request.set_message(user);

  // Container for the data we expect from the server.
  ClientResponse reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  ClientContext context;

  // The actual RPC.
  Status status = stub_->CallToCluster(&context, request, &reply);

  // Act upon its status.
  if (status.ok()) {
    return reply.message();
  } else {
    std::cout << status.error_code() << ": " << status.error_message()
              << std::endl;
    return "RPC failed";
  }
}

