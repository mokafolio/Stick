#ifndef STICK_TIMEPOINT_HPP
#define STICK_TIMEPOINT_HPP

#include <Stick/Duration.hpp>

namespace stick
{
template <class C, class T>
class TimePointT
{
  public:
    typedef C Clock;
    typedef T Rep;

    inline TimePointT() : m_value(0)
    {
    }

    inline TimePointT(const Rep & _val) : m_value(_val)
    {
    }

    inline Duration operator-(const TimePointT & _b)
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
} // namespace stick

#endif // STICK_TIMEPOINT_HPP
