#ifndef STICK_STICKSYSTEMCLOCK_HPP
#define STICK_STICKSYSTEMCLOCK_HPP

#include <Stick/TimePoint.hpp>

namespace stick
{
    class SystemClock
    {
    public:

#if STICK_PLATFORM == STICK_PLATFORM_OSX
        typedef uint64_t NativeRep;
#endif //STICK_PLATFORM == STICK_PLATFORM_OSX

        typedef TimePointT<SystemClock, NativeRep> TimePoint;

        static TimePoint now();
    };
}

#endif //STICK_STICKSYSTEMCLOCK_HPP
