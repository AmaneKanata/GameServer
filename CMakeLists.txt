cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

project ("GameServer")

file(GLOB _all_proto_files ${CMAKE_CURRENT_SOURCE_DIR}/src/ProtoBuf/ProtoFiles/*.proto)
SET(PROTOBUF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/ProtoBuf)

foreach(__proto_file ${_all_proto_files})
    get_filename_component(__current_proto ${__proto_file} NAME)
    if (WIN32)
        execute_process(COMMAND protoc -I=./ProtoFiles --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/src/Server/src/Protocol ${__current_proto} WORKING_DIRECTORY ${PROTOBUF_DIR}) 
        execute_process(COMMAND protoc -I=./ProtoFiles --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/src/DummyClient/src/Protocol ${__current_proto} WORKING_DIRECTORY ${PROTOBUF_DIR}) 
    endif (WIN32)
endforeach(__proto_file)

if (WIN32)
    execute_process(COMMAND Generator.exe --path=./ProtoFiles --output=${CMAKE_CURRENT_SOURCE_DIR}/src/Server/src/Contents/Base/PacketHandler.h --recv=C_ --send=S_ --template PacketHandler.h WORKING_DIRECTORY ${PROTOBUF_DIR})
    execute_process(COMMAND Generator.exe --path=./ProtoFiles --output=${CMAKE_CURRENT_SOURCE_DIR}/src/Server/src/PacketManager.h --recv=C_ --send=S_ --template PacketManager.h WORKING_DIRECTORY ${PROTOBUF_DIR})
    execute_process(COMMAND Generator.exe --path=./ProtoFiles --output=${CMAKE_CURRENT_SOURCE_DIR}/src/Server/src/PacketManager.cpp --recv=C_ --send=S_ --template PacketManager.cpp WORKING_DIRECTORY ${PROTOBUF_DIR})
    execute_process(COMMAND Generator.exe --path=./ProtoFiles --output=${CMAKE_CURRENT_SOURCE_DIR}/src/Server/src/Protocols.h --template Protocols.h WORKING_DIRECTORY ${PROTOBUF_DIR})

    execute_process(COMMAND Generator.exe --path=./ProtoFiles --output=${CMAKE_CURRENT_SOURCE_DIR}/src/DummyClient/src/PacketManager.h --recv=S_ --send=C_ --template PacketManager.h WORKING_DIRECTORY ${PROTOBUF_DIR})
    execute_process(COMMAND Generator.exe --path=./ProtoFiles --output=${CMAKE_CURRENT_SOURCE_DIR}/src/DummyClient/src/Protocols.h --template Protocols.h WORKING_DIRECTORY ${PROTOBUF_DIR})
endif (WIN32)

add_subdirectory (src/CoreLib)
add_subdirectory (src/Server)
add_subdirectory (src/DummyClient)

add_dependencies(Server CoreLib)
add_dependencies(DummyClient CoreLib)
