#ifndef STICK_PLATFORM_HPP
#define STICK_PLATFORM_HPP

//ASSERT
#if defined(DEBUG) || defined(_DEBUG)
#define STICK_ASSERT( exp ) assert( exp )
#else
#define STICK_ASSERT( exp )
#endif

//DEBUG
#if defined(DEBUG) || defined(_DEBUG)
#define STICK_DEBUG
#else
#undef STICK_DEBUG
#endif

//POSSIBLE PLATFORMS
#define STICK_PLATFORM_OSX 1
#define STICK_PLATFORM_LINUX 2

#if defined ( __APPLE__ )
#define STICK_PLATFORM STICK_PLATFORM_OSX
#define STICK_PLATFORM_UNIX
#elif __linux__
#define STICK_PLATFORM STICK_PLATFORM_LINUX
#define STICK_PLATFORM_UNIX
#endif

//LINE AND FILE MACROS
#define STICK_LINE __LINE__
#define STICK_FILE __FILE__

//visibility
#define STICK_API  __attribute__ ((visibility("default")))
#define STICK_LOCAL __attribute__ ((visibility("hidden")))

#include <stdint.h>
#include <stdio.h>

namespace stick
{
    typedef uint8_t UInt8;
    typedef uint16_t UInt16;
    typedef uint32_t UInt32 ;
    typedef uint64_t UInt64;

    typedef int8_t Int8;
    typedef int16_t Int16;
    typedef int32_t Int32;
    typedef int64_t Int64;
    typedef size_t Size;
    typedef intmax_t IntMax;
    typedef uintmax_t UIntMax;

    typedef float Float32;
    typedef double Float64;
}

#endif //STICK_PLATFORM_HPP
