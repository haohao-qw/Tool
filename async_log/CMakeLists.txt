cmake_minimum_required(VERSION 3.2)
project(ASYNC_LOG)

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




#set(LIB_SOUR
#src/buffer.cc
	#	src/async_log.cc
	#       )

	#add_library(ASYNC_LOG STATIC ${LIB_SOUR})
	
	#set(TAR_LIB
	#ASYNC_LOG
	#pthread
	#)



###########################TESTS########################

#add_executable(ex_test tests/ex_test.cc)
#target_link_libraries(ex_test ${TAR_LIB})


#add_executable(ex_time tests/ex_time.cc)
#target_link_libraries(ex_time ${TAR_LIB})

add_executable(ex_buffer tests/ex_buffer.cc)
target_link_libraries(ex_buffer ${TAR_LIB})


#add_executable(ex_log tests/ex_log.cc)
#target_link_libraries(ex_log ${TAR_LIB})



