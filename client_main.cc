
#include "network/raft_client.h"

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  raft::RaftClient client(grpc::CreateChannel(
      "localhost:50000", grpc::InsecureChannelCredentials()));
  std::string user("world");
  std::string reply = client.SendRequest(user);
  std::cout << "Greeter received: " << reply << std::endl;

  return 0;
}
