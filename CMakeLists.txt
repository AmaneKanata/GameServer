cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

project ("GameServer")

add_subdirectory (src/CoreLib)
add_subdirectory (src/Server)

add_dependencies(Server CoreLib)