project('tutorial', 'c', 'cpp', default_options : ['c_std=c11', 'cpp_std=c++14'])

# disable this bogus warning (should be deprecated in newer clang versions)
add_global_arguments('-Wno-missing-braces', language: 'cpp')

stickInc = [
    'Stick/Allocators/AllocatorUtilities.hpp',
    'Stick/Allocators/Block.hpp',
    'Stick/Allocators/Bucketizer.hpp',
    'Stick/Allocators/FallbackAllocator.hpp',
    'Stick/Allocators/FreeListAllocator.hpp',
    'Stick/Allocators/GlobalAllocator.hpp',
    'Stick/Allocators/LinearAllocator.hpp',
    'Stick/Allocators/Mallocator.hpp',
    'Stick/Allocators/MemoryChunk.hpp',
    'Stick/Allocators/NoAllocator.hpp',
    'Stick/Allocators/PoolAllocator.hpp',
    'Stick/Allocators/Segregator.hpp',
    'Stick/Allocator.hpp',
    'Stick/ArgumentParser.hpp',
    'Stick/CallbackID.hpp',
    'Stick/ConditionVariable.hpp',
    'Stick/DefaultCleanup.hpp',
    'Stick/Duration.hpp',
    'Stick/DynamicArray.hpp',
    'Stick/Error.hpp',
    'Stick/ErrorCategory.hpp',
    'Stick/ErrorCodes.hpp',
    'Stick/Event.hpp',
    'Stick/EventForwarder.hpp',
    'Stick/EventPublisher.hpp',
    'Stick/FileSystem.hpp',
    'Stick/FileUtilities.hpp',
    'Stick/FixedArray.hpp',
    'Stick/Hash.hpp',
    'Stick/HashMap.hpp',
    'Stick/HighResolutionClock.hpp',
    'Stick/Iterator.hpp',
    'Stick/Map.hpp',
    'Stick/Maybe.hpp',
    'Stick/Mutex.hpp',
    'Stick/Path.hpp',
    'Stick/Platform.hpp',
    'Stick/Print.hpp',
    'Stick/RBTree.hpp',
    'Stick/Result.hpp',
    'Stick/ScopedLock.hpp',
    'Stick/SharedPtr.hpp',
    'Stick/StaticArray.hpp',
    'Stick/String.hpp',
    'Stick/StringConversion.hpp',
    'Stick/SystemClock.hpp',
    'Stick/Test.hpp',
    'Stick/Thread.hpp',
    'Stick/TimePoint.hpp',
    'Stick/TypeInfo.hpp',
    'Stick/TypeList.hpp',
    'Stick/UniquePtr.hpp',
    'Stick/URI.hpp',
    'Stick/Utility.hpp',
    'Stick/Variant.hpp']

# not sure if there is a cleaner way with meson to only have one array of headers and maintain the Private subdir
privateInc = [
    'Stick/Private/Callback.hpp',
    'Stick/Private/FunctionTraits.hpp',
    'Stick/Private/IndexSequence.hpp',
    'Stick/Private/MappedCallbackStorage.hpp',
    'Stick/Private/MurmurHash2.hpp']

stickSrc = ['Stick/ArgumentParser.cpp', 
            'Stick/ConditionVariable.cpp',
            'Stick/Error.cpp',
            'Stick/ErrorCategory.cpp',
            'Stick/Event.cpp',
            'Stick/FileSystem.cpp',
            'Stick/FileUtilities.cpp',
            'Stick/HighResolutionClock.cpp',
            'Stick/Mutex.cpp',
            'Stick/Path.cpp',
            'Stick/SystemClock.cpp',
            'Stick/Thread.cpp',
            'Stick/TypeInfo.cpp',
            'Stick/URI.cpp']

threadsDep = dependency('threads')

install_headers(stickInc, subdir: 'Stick')
install_headers(privateInc, subdir: 'Stick/Private')

if get_option('static')
    stick = static_library('Stick', stickSrc, dependencies: threadsDep, install: true)
else
    stick = shared_library('Stick', stickSrc, dependencies: threadsDep, install: true)
endif

stickDep = declare_dependency(link_with : stick)

tests = executable('StickTests', 'Tests/StickTests.cpp', dependencies: stickDep)
test('Stick Tests', tests)
