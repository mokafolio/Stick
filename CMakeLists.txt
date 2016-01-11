cmake_minimum_required (VERSION 2.8.11)
project (Stick)
set(CMAKE_CXX_FLAGS "-std=c++11 -fno-exceptions")
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/..)

set (STICKDEPS pthread)

set (STICKINC 
Allocator.hpp
ConditionVariable.hpp
Duration.hpp
DynamicArray.hpp
Error.hpp
ErrorCategory.hpp
ErrorCodes.hpp
FileUtilities.hpp
HashMap.hpp
HighResolutionClock.hpp
Iterator.hpp
Map.hpp
Mutex.hpp
Path.hpp
Platform.hpp
Print.hpp
RBTree.hpp
Result.hpp
ScopedLock.hpp
String.hpp
SystemClock.hpp
Test.hpp
Thread.hpp
TimePoint.hpp
TypeInfo.hpp
URI.hpp
Utility.hpp
)

set (STICKSRC 
ConditionVariable.cpp
Error.cpp 
ErrorCategory.cpp
FileUtilities.cpp
HighResolutionClock.cpp 
Mutex.cpp 
Path.cpp
SystemClock.cpp
Thread.cpp 
TypeInfo.cpp
URI.cpp
)

add_library (Stick SHARED ${STICKSRC})
add_library (StickStatic STATIC ${STICKSRC})
install (TARGETS Stick DESTINATION /usr/local/lib)
install (FILES ${STICKINC} DESTINATION /usr/local/include/Stick)
add_subdirectory (Tests)
