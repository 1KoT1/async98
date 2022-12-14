cmake_minimum_required(VERSION 3.10)

project(udpEcho LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Poco 1.7.2 REQUIRED Foundation)

if(NOT DEFINED coroutine03_SOURCE_DIR)
	add_subdirectory(../../Core ${CMAKE_BINARY_DIR}/coroutine03 EXCLUDE_FROM_ALL)
endif(NOT DEFINED coroutine03_SOURCE_DIR)

if(NOT DEFINED coroutine03Net_SOURCE_DIR)
	add_subdirectory(../../Net ${CMAKE_BINARY_DIR}/coroutine03Net EXCLUDE_FROM_ALL)
endif(NOT DEFINED coroutine03Net_SOURCE_DIR)

add_executable(udpEchoServer
	server/udpEchoServer.cpp
	)

target_include_directories(udpEchoServer PUBLIC
	include
	)

target_link_libraries(udpEchoServer PUBLIC
	Poco::Foundation
	coroutine03
	coroutine03Net
	)

add_executable(udpEchoClient
	client/udpEchoClient.cpp
	)

target_include_directories(udpEchoClient PUBLIC
	include
	)

target_link_libraries(udpEchoClient PUBLIC
	Poco::Foundation
	coroutine03
	coroutine03Net
	)
