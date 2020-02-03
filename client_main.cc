
#include "network/raft_client.h"
#include <thread>

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  raft::RaftClient client(grpc::CreateChannel(
        "localhost:50000", grpc::InsecureChannelCredentials()));

  // Spawn reader thread that loops indefinitely
  std::thread thread_ = std::thread(&raft::RaftClient::AsyncCompleteRpc, &client);

  for (int i = 0; i < 10; i++) {
    std::string user("world " + std::to_string(i));
    client.AsyncSendRequest(user);  // The actual RPC call!
  }

  std::cout << "Press control-c to quit" << std::endl << std::endl;
  thread_.join();  //blocks forever
  return 0;
}
