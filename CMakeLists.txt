
cmake_minimum_required(VERSION 2.8)

project(raft C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#set(CMAKE_BUILD_TYPE Debug)

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_subdirectory(network)
add_subdirectory(utils)
add_subdirectory(core)

add_executable(raft_main "main.cc")
target_link_libraries(raft_main
  raft_core
  raft_network
  raft_utils)
add_executable(raft_client_main "client_main.cc")
target_link_libraries(raft_client_main
  raft_network
  raft_utils)
