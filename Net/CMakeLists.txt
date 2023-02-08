cmake_minimum_required(VERSION 3.10)

project(asynch98Net LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Poco 1.7.2 REQUIRED Foundation Net)

if(NOT DEFINED asynch98_SOURCE_DIR)
	add_subdirectory(../Core ${CMAKE_BINARY_DIR}/asynch98 EXCLUDE_FROM_ALL)
endif(NOT DEFINED asynch98_SOURCE_DIR)

add_library(asynch98Net
	src/Asynch98/Net/SocketUdp.cpp
	)

target_include_directories(asynch98Net PUBLIC
	include
	)

target_link_libraries(asynch98Net PUBLIC
	Poco::Foundation
	Poco::Net
	asynch98
	)
