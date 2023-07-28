# Minimum CMake required
cmake_minimum_required(VERSION 3.15)

# Project
project(Hangman)

# Set the MY_INSTALL_DIR variable to the desired directory path
set(MY_INSTALL_DIR "$ENV{HOME}/.local")
list(APPEND CMAKE_PREFIX_PATH "${MY_INSTALL_DIR}")
list(APPEND CMAKE_PREFIX_PATH "${MY_INSTALL_DIR}/include")

option(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG)
find_package(gRPC CONFIG)


if(gRPC_FOUND)
  message(STATUS "Using gRPC ${gRPC_VERSION}")
  set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
  set(_REFLECTION gRPC::grpc++_reflection)
  set(_GRPC_GRPCPP gRPC::grpc++)
else()
  message("gRPC NOT FOUND, Using FetchContent")
  include(FetchContent)
  FetchContent_Declare(
    grpc
    GIT_REPOSITORY https://github.com/grpc/grpc
    GIT_TAG        v1.56.2
  )
  set(FETCHCONTENT_QUIET OFF)
  FetchContent_MakeAvailable(grpc)
  set(_PROTOBUF_LIBPROTOBUF libprotobuf)
  set(_REFLECTION grpc++_reflection)
  set(_GRPC_GRPCPP grpc++)
endif()


if(CMAKE_CROSSCOMPILING)
  find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
endif()

set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib")
include_directories(${LIB_DIR})

file(GLOB LIB_FILES "${LIB_DIR}/*.cc" "${LIB_DIR}/*.h")

add_library(grpc_proto_files ${LIB_FILES})
target_link_libraries(grpc_proto_files
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})


foreach(_target
  hangman_client hangman_server)
  add_executable(${_target} "${_target}.cc")
  target_link_libraries(${_target}
    grpc_proto_files
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
endforeach()

