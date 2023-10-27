cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE src ./src/*.h ./src/*.cpp)

add_library (CoreLib STATIC ${src})

target_include_directories(CoreLib PUBLIC src)
target_include_directories(CoreLib PUBLIC src/Job)
target_include_directories(CoreLib PUBLIC src/Network)
target_include_directories(CoreLib PUBLIC src/Thread)

if (WIN32)
	#set(BOOST_ROOT "C:/Boost")
	set(CMAKE_PREFIX_PATH "C:/Boost" ${CMAKE_PREFIX_PATH})
	set(Boost_USE_STATIC_LIBS ON)
endif (WIN32)

find_package(Boost COMPONENTS thread REQUIRED)

target_include_directories(CoreLib PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(CoreLib PUBLIC ${Boost_LIBRARIES})