#include <Stick/HighResolutionClock.hpp>

#if STICK_PLATFORM == STICK_PLATFORM_OSX
#include <mach/mach_time.h>
#elif STICK_PLATFORM == STICK_PLATFORM_LINUX
#include <time.h>
#endif //STICK_PLATFORM

namespace stick
{

#if STICK_PLATFORM == STICK_PLATFORM_OSX

    struct MachHelper
    {
        MachHelper()
        {
            mach_timebase_info(&info);
            factor = static_cast<double>(info.numer) / info.denom;
        }

        struct mach_timebase_info info;
        Float64 factor;
    };

    static MachHelper s_machHelper;

    HighResolutionClock::TimePoint HighResolutionClock::now()
    {
        return TimePoint(mach_absolute_time() * s_machHelper.factor);
    }

#elif STICK_PLATFORM == STICK_PLATFORM_LINUX

    HighResolutionClock::TimePoint HighResolutionClock::now()
    {
        timespec ts;
        if(clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
        {
            return TimePoint(ts.tv_sec * 1000000000 + ts.tv_nsec);
        }
        return TimePoint();
    }

#endif //STICK_PLATFORM


}
