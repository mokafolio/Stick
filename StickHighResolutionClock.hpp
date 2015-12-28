#ifndef STICK_HIGHRESOLUTIONCLOCK_HPP
#define STICK_HIGHRESOLUTIONCLOCK_HPP

#include <Stick/StickDuration.hpp>

#if STICK_PLATFORM == STICK_PLATFORM_OSX
#include <mach/mach_time.h>
#endif

namespace stick
{   
    template<class T>
    class TimePointT
    {
    public:

        typedef T Rep;

        TimePointT(const Rep & _val) :
        m_value(_val)
        {

        }


    private:

        Rep m_value;
    };

    class HighResolutionClock
    {
    public:
#if STICK_PLATFORM == STICK_PLATFORM_OSX
        typedef uint64_t NativeRep;
#endif
        typedef TimePointT<NativeRep> TimePoint;

        TimePoint now() const;
    };
}

#endif //STICK_HIGHRESOLUTIONCLOCK_HPP
