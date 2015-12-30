#ifndef STICK_HIGHRESOLUTIONCLOCK_HPP
#define STICK_HIGHRESOLUTIONCLOCK_HPP

#include <Stick/StickTimePoint.hpp>

#if STICK_PLATFORM == STICK_PLATFORM_OSX
#include <mach/mach_time.h>
#endif

namespace stick
{
    class HighResolutionClock
    {
    public:

#if STICK_PLATFORM == STICK_PLATFORM_OSX
        typedef uint64_t NativeRep;
#endif //STICK_PLATFORM == STICK_PLATFORM_OSX

        typedef TimePointT<HighResolutionClock, NativeRep> TimePoint;

        static TimePoint now();
    };
}

#endif //STICK_HIGHRESOLUTIONCLOCK_HPP
