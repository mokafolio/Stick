#ifndef STICK_STICKTIMEPOINT_HPP
#define STICK_STICKTIMEPOINT_HPP

#include <Stick/StickDuration.hpp>

namespace stick
{
    template<class C, class T>
    class TimePointT
    {
    public:

        typedef C Clock;
        typedef T Rep;

        inline TimePointT(const Rep & _val) :
            m_value(_val)
        {

        }

        inline Duration operator - (const TimePointT & _b)
        {
            return Duration::fromNanoseconds(m_value - _b.m_value);
        }

        inline Duration timeSinceEpoch() const
        {
            return m_value;
        }

    private:

        Rep m_value;
    };
}

#endif //STICK_STICKTIMEPOINT_HPP
