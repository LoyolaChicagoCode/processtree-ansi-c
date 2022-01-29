cmake_minimum_required(VERSION 3.14)

project(ProcessTreeExamples)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(fakeps-simple)
add_subdirectory(fakeps-vector)
add_subdirectory(processtree-c)
