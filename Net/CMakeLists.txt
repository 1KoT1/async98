cmake_minimum_required(VERSION 3.10)

project(coroutine03Net LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Poco 1.7.2 REQUIRED Foundation Net)

if(NOT DEFINED coroutine03_SOURCE_DIR)
	add_subdirectory(../Core ${CMAKE_BINARY_DIR}/coroutine03 EXCLUDE_FROM_ALL)
endif(NOT DEFINED coroutine03_SOURCE_DIR)

add_library(coroutine03Net
	src/Coroutine03/Net/SocketUdp.cpp
	)

target_include_directories(coroutine03Net PUBLIC
	include
	)

target_link_libraries(coroutine03Net PUBLIC
	Poco::Foundation
	Poco::Net
	coroutine03
	)
