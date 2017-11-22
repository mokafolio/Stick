cmake_minimum_required (VERSION 2.8.11)
project (Stick)
set(CMAKE_CXX_FLAGS "-std=c++11 -fno-exceptions")
include_directories (${CMAKE_CURRENT_SOURCE_DIR})
option(AddTests "AddTests" ON)

set (STICKDEPS pthread)

set (STICKINC 
Stick/Allocators/AllocatorUtilities.hpp
Stick/Allocators/Block.hpp
Stick/Allocators/Bucketizer.hpp
Stick/Allocators/FallbackAllocator.hpp
Stick/Allocators/FreeListAllocator.hpp
Stick/Allocators/GlobalAllocator.hpp
Stick/Allocators/LinearAllocator.hpp
Stick/Allocators/Mallocator.hpp
Stick/Allocators/MemoryChunk.hpp
Stick/Allocators/NoAllocator.hpp
Stick/Allocators/PoolAllocator.hpp
Stick/Allocators/Segregator.hpp
Stick/Allocator.hpp
Stick/ArgumentParser.hpp
Stick/CallbackID.hpp
Stick/ConditionVariable.hpp
Stick/Duration.hpp
Stick/DynamicArray.hpp
Stick/Error.hpp
Stick/ErrorCategory.hpp
Stick/ErrorCodes.hpp
Stick/Event.hpp
Stick/EventForwarder.hpp
Stick/EventPublisher.hpp
Stick/FileSystem.hpp
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
Stick/TypeList.hpp
Stick/UniquePtr.hpp
Stick/URI.hpp
Stick/Utility.hpp
Stick/Variant.hpp
Stick/Private/Callback.hpp
Stick/Private/FunctionTraits.hpp
Stick/Private/IndexSequence.hpp
Stick/Private/MappedCallbackStorage.hpp
Stick/Private/MurmurHash2.hpp
)

set (STICKSRC 
Stick/ArgumentParser.cpp
Stick/ConditionVariable.cpp
Stick/Error.cpp 
Stick/ErrorCategory.cpp
Stick/Event.cpp
Stick/FileSystem.cpp
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

if(AddTests)
    add_subdirectory (Tests)
endif()

install (TARGETS Stick StickStatic DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
foreach ( file ${STICKINC} )
    get_filename_component( dir ${file} DIRECTORY )
    install( FILES ${file} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${dir} )
endforeach()
