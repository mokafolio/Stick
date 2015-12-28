cmake_minimum_required (VERSION 2.8.11)
project (Stick)
set(CMAKE_CXX_FLAGS "-std=c++11 -fno-exceptions")
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

set (STICKDEPS pthread)
set (STICKINC 
StickAllocator.hpp
StickConditionVariable.hpp
StickDynamicArray.hpp
StickError.hpp
StickErrorCategory.hpp
StickErrorCodes.hpp
StickHashMap.hpp
StickIterator.hpp
StickMap.hpp
StickMutex.hpp
StickPlatform.hpp
StickPrint.hpp
StickRBTree.hpp
StickScopedLock.hpp
StickString.hpp
StickTest.hpp
StickThread.hpp
StickUtility.hpp
)
set (STICKSRC StickError.cpp StickErrorCategory.cpp StickThread.cpp StickMutex.cpp StickConditionVariable.cpp)

add_library (Stick ${STICKSRC})
install (TARGETS Stick DESTINATION /usr/local/lib)
install (FILES ${STICKINC} DESTINATION /usr/local/include/Stick)
add_subdirectory (Tests)
