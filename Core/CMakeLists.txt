cmake_minimum_required(VERSION 3.10)

project(asynch98 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Poco 1.7.2 REQUIRED Foundation)

add_library(asynch98
	src/Asynch98/Core/DispatcherEPoll.cpp
	src/Asynch98/Core/EPoll.cpp
	src/Asynch98/Core/HandlerEPollEvents.cpp
	src/Asynch98/Core/StopBySignals.cpp
	src/Asynch98/Core/CanceledException.cpp
	)

target_include_directories(asynch98 PUBLIC
	include
	)

target_link_libraries(asynch98 PUBLIC
	Poco::Foundation
	)
