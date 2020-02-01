
cmake_minimum_required(VERSION 2.8)

project(raft C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_subdirectory(grpc)
add_subdirectory(utils)
add_subdirectory(core)

add_executable(raft_main "main.cc")
target_link_libraries(raft_main
  raft_core
  raft_client
  raft_server
  utils)
