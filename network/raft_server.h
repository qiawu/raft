
#ifndef RAFT_SERVER_H
#define RAFT_SERVER_H

#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <grpcpp/grpcpp.h>

#include "network/raft.grpc.pb.h"
#include "node_address.h"
#include "message.h"
#include "call_data.h"

namespace raft {
  // Logic and data behind the server's behavior.
  class RaftServer {
    public:
      RaftServer(const NodeAddress& addr, HandleFunc handle_func): addr_(addr), handle_func_(handle_func), is_server_shutting_down_(false) {}
      ~RaftServer();
      Status Start();
      Status Shutdown();
    private:
      Status GetClientAddress(grpc::ServerContext* context, NodeAddress& out_addr);
      void HandleRpcs();

      NodeAddress addr_;
      HandleFunc handle_func_;

      RaftService::AsyncService async_service_;
      std::unique_ptr<grpc::Server> async_server_;
      std::unique_ptr<grpc::ServerCompletionQueue> async_cq_;
      std::thread bg_handler_;
      std::atomic<bool> is_server_shutting_down_;
  };

}

#endif
