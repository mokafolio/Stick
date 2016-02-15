cmake_minimum_required (VERSION 2.8.11)
project (Stick)
set(CMAKE_CXX_FLAGS "-std=c++11 -fno-exceptions")
include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (STICKDEPS pthread)

set (STICKINC 
Stick/Allocator.hpp
Stick/ConditionVariable.hpp
Stick/Duration.hpp
Stick/DynamicArray.hpp
Stick/Error.hpp
Stick/ErrorCategory.hpp
Stick/ErrorCodes.hpp
Stick/FileUtilities.hpp
Stick/HashMap.hpp
Stick/HighResolutionClock.hpp
Stick/Iterator.hpp
Stick/Map.hpp
Stick/Maybe.hpp
Stick/Mutex.hpp
Stick/Path.hpp
Stick/Platform.hpp
Stick/Print.hpp
Stick/RBTree.hpp
Stick/Result.hpp
Stick/ScopedLock.hpp
Stick/String.hpp
Stick/StringConversion.hpp
Stick/SystemClock.hpp
Stick/Test.hpp
Stick/Thread.hpp
Stick/TimePoint.hpp
Stick/TypeInfo.hpp
Stick/UniquePtr.hpp
Stick/URI.hpp
Stick/Utility.hpp
)

set (STICKDETAILINC
Stick/Detail/MurmurHash2.hpp
)

set (STICKSRC 
Stick/ConditionVariable.cpp
Stick/Error.cpp 
Stick/ErrorCategory.cpp
Stick/FileUtilities.cpp
Stick/HighResolutionClock.cpp 
Stick/Mutex.cpp 
Stick/Path.cpp
Stick/SystemClock.cpp
Stick/Thread.cpp 
Stick/TypeInfo.cpp
Stick/URI.cpp
)

add_library (Stick SHARED ${STICKSRC})
add_library (StickStatic STATIC ${STICKSRC})
add_subdirectory (Tests)
install (TARGETS Stick StickStatic DESTINATION /usr/local/lib)
install (FILES ${STICKINC} DESTINATION /usr/local/include/Stick)
install (FILES ${STICKDETAILINC} DESTINATION /usr/local/include/Stick/Detail)
