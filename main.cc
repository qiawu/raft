
#include "core/raft_node.h"

using namespace std;
int main(int argc, char * argv[]) {
  raft::RaftNode node;
  raft::Status s = node.Initialize("test");
  cout << "node initialize" << endl;
  return 0;
}
