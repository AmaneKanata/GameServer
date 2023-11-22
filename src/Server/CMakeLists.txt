cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE src ./src/*.h ./src/*.cpp ./src/*.cc)

add_executable (Server ${src} "src/Util/Convert.h")

target_include_directories(Server PUBLIC ./Include ../CoreLib/src)

target_include_directories(Server PUBLIC src)
target_include_directories(Server PUBLIC src/Contents src/Contents/Base src/Contents/GameObject src/Contents/FPS)
target_include_directories(Server PUBLIC src/HTTP)
target_include_directories(Server PUBLIC src/Protocol)
target_include_directories(Server PUBLIC src/Session)
target_include_directories(Server PUBLIC src/Util)

target_link_libraries(Server PRIVATE CoreLib)

if (WIN32)
    set(CMAKE_PREFIX_PATH "C:/Program Files/gRPC" ${CMAKE_PREFIX_PATH})
    set(CMAKE_PREFIX_PATH "C:/Program Files/agones" ${CMAKE_PREFIX_PATH})
    set(CMAKE_PREFIX_PATH "C:/Program Files/BULLET_PHYSICS" ${CMAKE_PREFIX_PATH})
    set(CMAKE_PREFIX_PATH "C:/Program Files/GLFW" ${CMAKE_PREFIX_PATH})
endif (WIN32)

find_package(Protobuf CONFIG)
target_link_libraries(Server PRIVATE protobuf::libprotobuf protobuf::libprotoc)

if (UNIX)
    find_package(absl CONFIG)
endif (UNIX)

find_package(gRPC CONFIG)
find_package(agones CONFIG)
target_link_libraries(Server PRIVATE gRPC::grpc++ agones)

find_package(httplib CONFIG REQUIRED)
target_link_libraries(Server PRIVATE httplib::httplib)

if(WIN32)
    target_include_directories(Server PUBLIC "C:/Program Files/BULLET_PHYSICS/include/bullet")
    target_link_libraries(Server PRIVATE 
        "C:/Program Files/BULLET_PHYSICS/lib/LinearMath.lib"
        "C:/Program Files/BULLET_PHYSICS/lib/BulletCollision.lib"
        "C:/Program Files/BULLET_PHYSICS/lib/BulletDynamics.lib"
    )
else()
    find_package(Bullet REQUIRED)
    target_include_directories(Server PRIVATE ${BULLET_INCLUDE_DIRS})
    target_link_libraries(Server PRIVATE ${BULLET_LIBRARIES})
endif()

if (WIN32)
    find_package(OpenGL REQUIRED)
    target_link_libraries(Server PRIVATE OpenGL::GL OpenGL::GLU)

    find_package(glfw3 CONFIG REQUIRED)
    target_link_libraries(Server PRIVATE glfw)
endif (WIN32)