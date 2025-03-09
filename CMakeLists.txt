cmake_minimum_required(VERSION 3.28)
project(cWeb)

add_subdirectory(client)
add_subdirectory(server)

SET(CMAKE_CC_FLAGS  "${CMAKE_CC_FLAGS} -Wall -Wextar -g3 -o0 -ggdb")
