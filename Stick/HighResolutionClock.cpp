#include <Stick/HighResolutionClock.hpp>

#if STICK_PLATFORM == STICK_PLATFORM_OSX
#include <mach/mach_time.h>
#endif //STICK_PLATFORM == STICK_PLATFORM_OSX

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

#endif //STICK_PLATFORM == STICK_PLATFORM_OSX
}
