cmake_minimum_required(VERSION 3.10)

project(tcpEcho LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Poco 1.7.2 REQUIRED Foundation)

if(NOT DEFINED asynch98_SOURCE_DIR)
	add_subdirectory(../../Core ${CMAKE_BINARY_DIR}/asynch98 EXCLUDE_FROM_ALL)
endif(NOT DEFINED asynch98_SOURCE_DIR)

if(NOT DEFINED asynch98Net_SOURCE_DIR)
	add_subdirectory(../../Net ${CMAKE_BINARY_DIR}/asynch98Net EXCLUDE_FROM_ALL)
endif(NOT DEFINED asynch98Net_SOURCE_DIR)

add_executable(tcpEchoServer
	server/tcpEchoServer.cpp
	)

target_include_directories(tcpEchoServer PUBLIC
	include
	)

target_link_libraries(tcpEchoServer PUBLIC
	Poco::Foundation
	asynch98
	asynch98Net
	)

add_executable(tcpEchoClient
	client/tcpEchoClient.cpp
	)

target_include_directories(tcpEchoClient PUBLIC
	include
	)

target_link_libraries(tcpEchoClient PUBLIC
	Poco::Foundation
	asynch98
	asynch98Net
	)
