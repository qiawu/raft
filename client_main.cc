
#include <thread>

#include "network/raft_client.h"
#include "network/node_address.h"

int main(int argc, char** argv) {
  raft::NodeAddress addr;
  raft::Status s = raft::NodeAddress::ParseNodeAddress("localhost:50000", addr);
  if (!s.ok()) {
    std::cout << s.ToString() << std::endl;
    return 1;
  }
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  raft::RaftClient client(addr);

  // Spawn reader thread that loops indefinitely
  std::thread thread_ = std::thread(&raft::RaftClient::AsyncCompleteRpc, &client);

  for (int i = 0; i < 10; i++) {
    raft::ClientRequestMessage msg("world " + std::to_string(i));
    client.AsyncCallToCluster(msg, [](const raft::Message&){});  // The actual RPC call!
  }

  std::cout << "Press control-c to quit" << std::endl << std::endl;
  thread_.join();  //blocks forever
  return 0;
}
