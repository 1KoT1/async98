cmake_minimum_required(VERSION 3.10)

project(helloWorld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Poco 1.7.2 REQUIRED Foundation)

if(NOT DEFINED libpoco_gtest_SOURCE_DIR)
	add_subdirectory(../../Core ${CMAKE_BINARY_DIR}/coroutine03 EXCLUDE_FROM_ALL)
endif(NOT DEFINED libpoco_gtest_SOURCE_DIR)

add_executable(helloWorld
	main.cpp
	)

target_include_directories(helloWorld PUBLIC
	include
	)

target_link_libraries(helloWorld PUBLIC
	Poco::Foundation
	coroutine03
	)
