set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++20 -g -lpthread -march=native -O0")
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")

include_directories(./include)
set(LIB_SOUR
        src/mutex.cc
        src/util.cc
        )
#add_library(free_lock_queue STATIC ${LIB_SOUR})
add_library(Utils SHARED ${LIB_SOUR})
#add_library(async_log STATIC ${LIB_SOUR})
#add_library(log SHARED ${LIB_SOUR})
