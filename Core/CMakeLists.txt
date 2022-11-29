cmake_minimum_required(VERSION 3.10)

project(coroutine03 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Poco 1.7.2 REQUIRED Foundation)

add_library(coroutine03
	src/Coroutine03/Core/DispatcherEPoll.cpp
	src/Coroutine03/Core/EPoll.cpp
	src/Coroutine03/Core/HandlerEPollEvents.cpp
	src/Coroutine03/Core/SocketUdp.cpp
	src/Coroutine03/Core/StopBySignals.cpp
	)

target_include_directories(coroutine03 PUBLIC
	include
	)

target_link_libraries(coroutine03 PUBLIC
	Poco::Foundation
	)
