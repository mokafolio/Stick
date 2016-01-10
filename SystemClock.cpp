#include <Stick/SystemClock.hpp>
#include <sys/time.h>

namespace stick
{
    SystemClock::TimePoint SystemClock::now()
    {
        timeval val;
        gettimeofday(&val, 0);
        return TimePoint(val.tv_sec * 1000000000 + val.tv_usec * 1000);
    }
}
