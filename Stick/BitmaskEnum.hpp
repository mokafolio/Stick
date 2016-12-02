#ifndef STICK_BITMASKENUM_HPP
#define STICK_BITMASKENUM_HPP

#include <type_traits>

namespace stick
{
    template<class E, class U = typename std::underlying_type<E>::type>
    class BitmaskEnum
    {
    public:

        BitmaskEnum() :
            m_flags(0)
        {
        }

        BitmaskEnum(E _f) :
            m_flags(_f)
        {
        }

        BitmaskEnum(const BitmaskEnum & _other) :
            m_flags(_other.m_flags)
        {
        }
        
        explicit operator bool()
        {
            return m_flags != 0;
        }

        BitmaskEnum & operator |= (E _val)
        {
            mFlags |= _val;
            return *this;
        }

        BitmaskEnum operator | (E _val)
        {
            BitmaskEnum result(*this);
            result |= _val;
            return result;
        }

        BitmaskEnum & operator &= (E _val)
        {
            m_flags &= _val;
            return *this;
        }

        BitmaskEnum operator &(E _val)
        {
            BitmaskEnum result(*this);
            result &= _val;
            return result;
        }

        BitmaskEnum operator ~()
        {
            BitmaskEnum result(*this);
            result.m_flags = ~result.m_flags;
            return result;
        }

    protected:

        U  m_flags;
    };
}

#endif //STICK_BITMASKENUM_HPP
