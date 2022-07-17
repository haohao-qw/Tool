cmake_minimum_required(VERSION 3.2)
project(NFT)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++20 -g  -march=native -O0")
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")

include_directories(./include)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#
#include_directories(Boost_INC /usr/include/boost)
#set(Boost_USE_STATIC_LIBS ON)
#set(BOOST_COMPONENTS date_time chrono filesystem iostreams
#        program_options regex system thread unit_test_framework)
#find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})




set(LIB_SOUR
	src/server.cc
#	src/client.cc
	src/Thread.cc
        )

add_library(NFT STATIC ${LIB_SOUR})

set(TAR_LIB
	NFT
	pthread
	)



###########################TESTS########################
#add_executable(ReadBuffer tests/ReadBufferTest.cc)
#target_link_libraries(ReadBuffer ${TAR_LIB})

#add_executable(Client tests/Client.cc)
#target_link_libraries(Client ${TAR_LIB})

add_executable(Server tests/Server.cc)
target_link_libraries(Server ${TAR_LIB})


