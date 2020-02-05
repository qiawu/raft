./kill_all_nodes.sh
./cmake/build/raft_main conf/node1.config > /tmp/node1.out &
./cmake/build/raft_main conf/node2.config > /tmp/node2.out &
./cmake/build/raft_main conf/node3.config > /tmp/node3.out &
