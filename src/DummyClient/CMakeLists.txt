cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE src ./src/*.h ./src/*.cpp ./src/*.cc)

add_executable (DummyClient ${src})

target_include_directories(DummyClient PUBLIC src)
target_include_directories(DummyClient PUBLIC src/Contents src/Contents/Base src/Contents/GameObject)
target_include_directories(DummyClient PUBLIC src/Protocol)
target_include_directories(DummyClient PUBLIC src/Session)
target_include_directories(DummyClient PUBLIC src/Util)

target_include_directories(DummyClient PRIVATE ../CoreLib/src)

target_link_libraries(DummyClient PRIVATE CoreLib)

if (WIN32)
    set(CMAKE_PREFIX_PATH "C:/Program Files/gRPC" ${CMAKE_PREFIX_PATH})
    set(CMAKE_PREFIX_PATH "C:/Program Files/agones" ${CMAKE_PREFIX_PATH})
endif (WIN32)

find_package(Protobuf REQUIRED)
target_include_directories(DummyClient PRIVATE ${Protobuf_INCLUDE_DIRS})
target_link_libraries(DummyClient PRIVATE ${Protobuf_LIBRARIES})
