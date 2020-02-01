
cmake_minimum_required(VERSION 2.8)

file(GLOB srcs "*.cc")

add_library(raft_core ${srcs})
target_link_libraries(raft_core
  raft_client
  raft_server
  utils)
