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

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

namespace raft {
  class RaftClient {
   public:
    RaftClient(std::shared_ptr<Channel> channel);

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::string SendRequest(const std::string& user);

   private:
    std::unique_ptr<RaftService::Stub> stub_;
  };

}
