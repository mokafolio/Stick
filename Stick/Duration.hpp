#ifndef STICK_DURATION_HPP
#define STICK_DURATION_HPP

#include <Stick/Platform.hpp>

namespace stick
{
    class STICK_API Duration
    {
    public:

        typedef Int64 Rep;

        typedef Float64 FloatRep;


        inline Duration() = default;

        static Duration fromHours(FloatRep _hours)
        {
            return fromMinutes(_hours * 60.0);
        }

        static Duration fromMinutes(FloatRep _minutes)
        {
            return fromSeconds(_minutes * 60.0);
        }

        static Duration fromSeconds(FloatRep _seconds)
        {
            return fromMilliseconds(_seconds * 1000.0);
        }

        static Duration fromMilliseconds(FloatRep _millis)
        {
            return Duration(_millis * 1000000);
        }

        static Duration fromMicroseconds(FloatRep _micros)
        {
            return Duration(_micros * 1000);
        }

        static Duration fromNanoseconds(Rep _nanos)
        {
            return Duration(_nanos);
        }

        inline FloatRep hours() const
        {
            return minutes() / 60.0;
        }

        inline FloatRep minutes() const
        {
            return seconds() / 60.0;
        }

        inline FloatRep seconds() const
        {
            return milliseconds() / static_cast<FloatRep>(1000.0);
        }

        inline FloatRep milliseconds() const
        {
            return microseconds() / static_cast<FloatRep>(1000.0);
        }

        inline FloatRep microseconds() const
        {
            return static_cast<FloatRep>(m_nanoseconds) / static_cast<FloatRep>(1000.0);
        }

        inline Rep nanoseconds() const
        {
            return m_nanoseconds;
        }

    private:

        inline Duration(Rep _nanos) :
        m_nanoseconds(_nanos)
        {

        }

        Rep m_nanoseconds;
    };
}

#endif //STICK_DURATION_HPP
