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

    inline Duration operator-(const Duration & _other) const
    {
        return Duration(m_nanoseconds - _other.m_nanoseconds);
    }

    inline Duration operator+(const Duration & _other) const
    {
        return Duration(m_nanoseconds + _other.m_nanoseconds);
    }

    template <class T>
    inline Duration operator*(T _scalar) const
    {
        return Duration(m_nanoseconds * _scalar);
    }

    template <class T>
    inline Duration operator/(T _scalar) const
    {
        return Duration(m_nanoseconds / _scalar);
    }

    inline Duration & operator-=(const Duration & _other)
    {
        m_nanoseconds -= _other.m_nanoseconds;
        return *this;
    }

    inline Duration & operator+=(const Duration & _other)
    {
        m_nanoseconds += _other.m_nanoseconds;
        return *this;
    }

    template <class T>
    inline Duration operator*=(T _scalar)
    {
        m_nanoseconds *= _scalar;
        return *this;
    }

    template <class T>
    inline Duration operator/=(T _scalar)
    {
        m_nanoseconds /= _scalar;
        return *this;
    }

    inline bool operator==(const Duration & _other) const
    {
        return m_nanoseconds == _other.m_nanoseconds;
    }

    inline bool operator!=(const Duration & _other) const
    {
        return m_nanoseconds != _other.m_nanoseconds;
    }

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
    inline Duration(Rep _nanos) : m_nanoseconds(_nanos)
    {
    }

    Rep m_nanoseconds;
};
} // namespace stick

#endif // STICK_DURATION_HPP
