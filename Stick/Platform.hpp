#ifndef STICK_PLATFORM_HPP
#define STICK_PLATFORM_HPP

#include <assert.h>

//ASSERT
#if !defined(NDEBUG)
#define STICK_ASSERT( exp ) assert( exp )
#else
#define STICK_ASSERT( exp )
#endif

//DEBUG
#if !defined(NDEBUG)
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

#ifdef STICK_PLATFORM_UNIX
#define STICK_PLATFORM_POSIX
#endif

//LINE AND FILE MACROS
#define STICK_LINE __LINE__
#define STICK_FILE __FILE__

//visibility
#define STICK_API  __attribute__ ((visibility("default")))
#define STICK_LOCAL __attribute__ ((visibility("hidden")))

// helper to define enum class and circumvent bug in gcc < version 6.0
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=43407#c6
#if defined(__GNUC__) && __GNUC__ < 6 && !defined(__clang__)
#define STICK_ENUM_CLASS_WITH_BASE(_attribute, _name, _base) enum class _name : _base _attribute
#define STICK_ENUM_WITH_BASE(_attribute, _name, _base) enum _name : _base _attribute
#else
#define STICK_ENUM_CLASS_WITH_BASE(_attribute, _name, _base) enum class _attribute _name : _base
#define STICK_ENUM_WITH_BASE(_attribute, _name, _base) enum _attribute _name : _base
#endif
#define STICK_API_ENUM_CLASS(_name) STICK_ENUM_CLASS_WITH_BASE(STICK_API, _name, stick::Int32)
#define STICK_LOCAL_ENUM_CLASS(_name) STICK_ENUM_CLASS_WITH_BASE(STICK_LOCAL, _name, stick::Int32)

#include <stdint.h>
#include <stdio.h>

namespace stick
{
    typedef uint8_t UInt8;
    typedef uint16_t UInt16;
    typedef uint32_t UInt32 ;
    typedef uint64_t UInt64;
    typedef uintptr_t UPtr;

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
