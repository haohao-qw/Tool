cmake_minimum_required(VERSION 3.2)
project(NFT)

# 设置release版本还是debug版本
if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    message(STATUS "Release版本")
    set(BuildType "Release")
else()
    set(BuildType "Debug")
    message(STATUS "Debug版本")
endif()

#设置编译选项
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "-std=c++20 -g  -march=native -O0")
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")

#设置输出 可执行文件以及lib库位置
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


#打印信息
message(STATUS "build type:")
message(STATUS "CXX_FLAGS = ${CMAKE_CXX_FLAGS} ")

#包含头文件信息
include_directories(${PROJECT_SOURCE_DIR}/async_log/include)
include_directories(${PROJECT_SOURCE_DIR}/Filets/include)
include_directories(${PROJECT_SOURCE_DIR}/free_lock_queue/include)

#添加子目录 递归Cmakelists 里层的先编译出库供外面的进行调用或者直接依靠源文件也可以进行编译
add_subdirectory(${PROJECT_SOURCE_DIR}/async_log)
add_subdirectory(${PROJECT_SOURCE_DIR}/free_lock_queue)
add_subdirectory(${PROJECT_SOURCE_DIR}/Filets)

set(LIBLIST
        free_lock_queue
        filets
        async_log
        pthread
        )

#编译可执行文件
add_executable(main main.cpp)
target_link_libraries(main ${LIBLIST})

add_executable(log example/log.cc)
target_link_libraries(log ${LIBLIST})

add_executable(log_buffer example/log_buffer.cc)
target_link_libraries(log_buffer ${LIBLIST})

add_executable(log_time example/log_time.cc)
target_link_libraries(log_time ${LIBLIST})

add_executable(pool_test example/pool_test.cc)
target_link_libraries(pool_test ${LIBLIST})

add_executable(Server example/Server.cc)
target_link_libraries(Server ${LIBLIST})


add_executable(Client example/Client.cc)
target_link_libraries(Client ${LIBLIST})











