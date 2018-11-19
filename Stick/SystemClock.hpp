#ifndef STICK_SYSTEMCLOCK_HPP
#define STICK_SYSTEMCLOCK_HPP

#include <Stick/TimePoint.hpp>

namespace stick
{
class SystemClock
{
  public:
#if STICK_PLATFORM == STICK_PLATFORM_OSX
    typedef uint64_t NativeRep;
#elif STICK_PLATFORM == STICK_PLATFORM_LINUX
    typedef uint64_t NativeRep;
#endif // STICK_PLATFORM

    typedef TimePointT<SystemClock, NativeRep> TimePoint;

    static TimePoint now();
};
} // namespace stick

#endif // STICK_SYSTEMCLOCK_HPP
