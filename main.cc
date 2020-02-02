
#include "core/raft_node.h"
#include "utils/configuration.h"

using namespace std;
int main(int argc, char * argv[]) {
  if (argc != 2) {
    std::cout << "a config full path must be provided" << std::endl;
    return 1;
  }
  raft::RaftNode node;
  raft::Status s = node.Initialize(argv[1]);
  if (!s.ok()) {
    std::cout << s.ToString() << std::endl;
    return 1;
  }
  
  std::cout << "waiting for client" << std::endl;
  std::cin.get();
  /*
  raft::Configuration config(argv[1]);
  s = config.Parse();
  if (!s.ok()) {
    std::cout << s.ToString() << std::endl;
    return 1;
  }
  config.PrettyPrint();
  */
  return 0;
}
