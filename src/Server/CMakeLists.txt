cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE src ./src/*.h ./src/*.cpp ./src/*.cc)

add_executable (Server ${src})

target_include_directories(Server PUBLIC ../CoreLib/src)

target_include_directories(Server PUBLIC src)
target_include_directories(Server PUBLIC src/Contents src/Contents/Base src/Contents/GameObject)
target_include_directories(Server PUBLIC src/Protocol)
target_include_directories(Server PUBLIC src/Session)
target_include_directories(Server PUBLIC src/Util)

target_link_libraries(Server PRIVATE CoreLib)

if (WIN32)
    set(CMAKE_PREFIX_PATH "C:/Program Files/gRPC" ${CMAKE_PREFIX_PATH})
    set(CMAKE_PREFIX_PATH "C:/Program Files/agones" ${CMAKE_PREFIX_PATH})
endif (WIN32)

find_package(Protobuf CONFIG)
target_link_libraries(Server PRIVATE protobuf::libprotobuf protobuf::libprotoc)

find_package(gRPC CONFIG)
find_package(agones CONFIG)
target_link_libraries(Server PRIVATE gRPC::grpc++ agones)