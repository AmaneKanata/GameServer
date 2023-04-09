cmake_minimum_required (VERSION 3.8)

file(GLOB _all_proto_files  ${CMAKE_CURRENT_SOURCE_DIR}/Proto/*.proto)
SET(_proto_gen_dir ${CMAKE_CURRENT_SOURCE_DIR}/src/Protocol)

foreach(__proto_file ${_all_proto_files})
    get_filename_component(__current_proto ${__proto_file} NAME)
    if (WIN32)
        execute_process(COMMAND protoc -I=${CMAKE_CURRENT_SOURCE_DIR}/Proto --cpp_out=${_proto_gen_dir} ${__current_proto} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}) 
    endif (WIN32)
endforeach(__proto_file)

if (WIN32)
    execute_process(COMMAND GenPackets.exe --path=${CMAKE_CURRENT_SOURCE_DIR}/Proto --output=${CMAKE_CURRENT_SOURCE_DIR}/src/Contents/Base/PacketHandler.h --recv=C_ --send=S_ --template PacketHandler.h WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    execute_process(COMMAND GenPackets.exe --path=${CMAKE_CURRENT_SOURCE_DIR}/Proto --output=${CMAKE_CURRENT_SOURCE_DIR}/src/PacketManager.h --recv=C_ --send=S_ --template PacketManager.h WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    execute_process(COMMAND GenPackets.exe --path=${CMAKE_CURRENT_SOURCE_DIR}/Proto --output=${CMAKE_CURRENT_SOURCE_DIR}/src/PacketManager.cpp --recv=C_ --send=S_ --template PacketManager.cpp WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif (WIN32)

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

    target_link_directories(Server PRIVATE /usr/lib/x86_64-linux-gnu)
    target_link_libraries(Server PRIVATE mysqlcppconn)
endif (UNIX)
