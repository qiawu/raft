
cmake_minimum_required(VERSION 2.8)

file(GLOB srcs "*.cc")

add_library(utils ${srcs})
