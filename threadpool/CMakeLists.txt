set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++20 -g -lpthread -march=native -O0")
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")

include_directories(./include)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(LIB_SOUR
        src/lock_threadpool.cc
		src/lockfree_queue.cc
		src/lockfree_threadpool.cc
		)
#add_library(free_lock_queue STATIC ${LIB_SOUR})
add_library(threadpool SHARED ${LIB_SOUR})