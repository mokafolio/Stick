cmake_minimum_required (VERSION 2.8.11)
project (Stick)
set(CMAKE_CXX_FLAGS "-std=c++11 -fno-exceptions")
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

set (STICKDEPS pthread)
set (STICKSRC StickError.cpp StickErrorCategory.cpp StickThread.cpp StickMutex.cpp StickConditionVariable.cpp)

add_library (Stick ${STICKSRC})
add_subdirectory (Tests)
