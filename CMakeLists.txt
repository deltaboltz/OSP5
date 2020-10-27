cmake_minimum_required(VERSION 2.8.4)
project(Project_4)

set(CMAKE_CXX_STANDARD 11)

add_executable(Project_4 src/oss.cpp include/clock_work.h include/sharedmemory.h src/sharedmemory.cpp include/childhandler.h src/childhandler.cpp include/errors.h src/errors.cpp include/filehandler.h src/filehander.cpp)

include_directories(include)