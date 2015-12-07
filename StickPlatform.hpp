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

//UNIT TESTING
#define SUITE(...) if(printf("\n------ " __VA_ARGS__),puts(""),true)
#define TEST(...)  (++tst,err+=!(ok=!!(__VA_ARGS__))),printf("[%s] %d %s \n",ok?" OK ":"FAIL",__LINE__,#__VA_ARGS__)
unsigned tst=0,err=0,ok=atexit([]{ SUITE("summary"){ printf("[%s] %d tests = %d passed + %d errors\n",err?"FAIL":" OK ",tst,tst-err,err); }});

#endif //STICK_PLATFORM_HPP
