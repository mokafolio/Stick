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

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

//UNIT TESTING
#define SUITE(...) if(printf("\n------ " __VA_ARGS__),puts(""),true)
#define TEST(...)  (++tst,err+=!(ok=!!(__VA_ARGS__))),printf("[%s] %d %s \n",ok?" OK ":"FAIL",__LINE__,#__VA_ARGS__)
unsigned tst=0,err=0,ok=atexit([]{ SUITE("summary"){ printf("[%s] %d tests = %d passed + %d errors\n",err?"FAIL":" OK ",tst,tst-err,err); }});

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

    typedef float Float32;
    typedef double Float64;
}

#endif //STICK_PLATFORM_HPP
