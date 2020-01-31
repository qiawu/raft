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

#include <grpcpp/grpcpp.h>

#include "raft.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

namespace raft {
  // Logic and data behind the server's behavior.
  class RaftServiceImpl final : public raft::RaftService::Service {
    Status HandleClientRequest(ServerContext* context, const raft::ClientRequest* request,
                    raft::ClientResponse* reply) override;
    Status HandleVote(ServerContext* context, const raft::VoteRequest* request,
                    raft::VoteResponse* reply) override;
    Status HandleReplication(ServerContext* context, const raft::ReplicateRequest* request,
                    raft::ReplicateResponse* reply) override;
  };

}

