#include <Stick/SystemClock.hpp>
#if STICK_PLATFORM == STICK_PLATFORM_OSX
#include <sys/time.h>
#elif STICK_PLATFORM == STICK_PLATFORM_LINUX
#include <time.h>
#endif // STICK_PLATFORM

namespace stick
{
#if STICK_PLATFORM == STICK_PLATFORM_OSX

SystemClock::TimePoint SystemClock::now()
{
    //@TODO use clock_gettime on osx sierra and later
    timeval val;
    gettimeofday(&val, 0);
    return TimePoint(val.tv_sec * 1000000000 + val.tv_usec * 1000);
}

#elif STICK_PLATFORM == STICK_PLATFORM_LINUX

SystemClock::TimePoint SystemClock::now()
{
    timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == 0)
    {
        return TimePoint(ts.tv_sec * 1000000000 + ts.tv_nsec);
    }

    return TimePoint();
}

#endif // STICK_PLATFORM
} // namespace stick
