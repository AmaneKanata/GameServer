cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE src ./src/*.h ./src/*.cpp ./src/*.cc)

add_executable (Server ${src})

target_include_directories(Server PRIVATE ../CoreLib/src ./Include)

target_link_libraries(Server PRIVATE CoreLib)

if (WIN32)
    file(GLOB libs ./Lib/*.lib)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        file(GLOB libs ./Lib/Win/Debug/*.lib)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        file(GLOB libs ./Lib/Win/Release/*.lib)
    endif()

    target_link_libraries(Server PRIVATE ${libs})
endif (WIN32)

if (UNIX)
    find_package(Protobuf REQUIRED)
    include_directories(${Protobuf_INCLUDE_DIRS})
    target_link_libraries(Server PRIVATE ${Protobuf_LIBRARIES})
endif (UNIX)
