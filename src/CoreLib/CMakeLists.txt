cmake_minimum_required (VERSION 3.8)

add_library (CoreLib STATIC 
	src/Job/LockQueue.cpp
	src/Job/LockQueue.h
	src/Job/Job.cpp
	src/Job/Job.h
	src/Job/JobTimer.cpp
	src/Job/JobTimer.h
	src/Job/GlobalQueue.cpp
	src/Job/GlobalQueue.h
	src/Job/JobQueue.h
	src/Job/JobQueue.cpp

	src/Network/Acceptor.cpp
	src/Network/Acceptor.h
	src/Network/Service.cpp
	src/Network/Service.h
	src/Network/Session.cpp
	src/Network/Session.h
	src/Network/RecvBuffer.h
	src/Network/RecvBuffer.cpp
	src/Network/SendBuffer.h
	src/Network/SendBuffer.cpp

	src/Thread/ThreadManager.cpp
	src/Thread/ThreadManager.h
	src/Thread/TLS.cpp
	src/Thread/TLS.h

	src/Util/LogManager.h
	src/Util/LogManager.cpp

	src/Singleton.h
	src/Singleton.cpp
	src/pch.h
)

if (WIN32)
	set(BOOST_ROOT "C:\\boost_1_81_0")
	set(Boost_USE_STATIC_LIBS ON)
endif (WIN32)

find_package(Boost COMPONENTS thread REQUIRED)

message(${Boost_INCLUDE_DIRS})
message(${Boost_LIBRARIES})

target_include_directories(CoreLib PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(CoreLib PUBLIC ${Boost_LIBRARIES})