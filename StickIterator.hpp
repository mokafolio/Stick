#ifndef STICK_ITERATOR_HPP
#define STICK_ITERATOR_HPP

namespace stick
{
    template<class T>
    struct IteratorTraits;

    template<class T>
    struct IteratorTraits<T*>
    {
        typedef T ValueType;
        typedef T & ReferenceType;
        typedef T * PointerType;
    };

    template<class T>
    struct IteratorTraits<const T*>
    {
        typedef const T ValueType;
        typedef const T & ReferenceType;
        typedef const T * PointerType;
    };

    template<class T>
    struct ReverseIterator
    {
        typedef IteratorTraits<T> IterTraits;

        typedef T Iter;

        typedef typename IterTraits::ValueType ValueType;

        typedef typename IterTraits::ReferenceType ReferenceType;

        typedef typename IterTraits::PointerType PointerType;


        explicit ReverseIterator(const Iter & _it) :
        m_it(_it)
        {

        }

        inline ReverseIterator & operator--() 
        {
            m_it++;
            return *this;
        } 

        inline ReverseIterator operator--(int)
        {
            ReverseIterator ret = *this;
            m_it++;
            return ret;
        }

        inline ReverseIterator & operator-=(Size _i) 
        {
            m_it += _i;
            return *this;
        }

        inline ReverseIterator operator-(Size _i) const
        {
            return m_it + _i;
        }

        inline ReverseIterator & operator++() 
        {
            m_it--;
            return *this;
        } 

        inline ReverseIterator operator++(int) 
        {
            ReverseIterator ret = *this;
            m_it--;
            return ret;
        }

        inline ReverseIterator & operator+=(Size _i) 
        {
            m_it -= _i;
            return *this;
        }

        inline ReverseIterator operator+(Size _i) const
        {
            return m_it - _i;
        }

        inline ReferenceType operator * () const
        {
            return *m_it;
        }

        inline PointerType operator -> () const
        {
            return m_it;
        }

        inline bool operator == (const ReverseIterator & _other) const
        {
            return m_it == _other.m_it;
        }

        inline bool operator != (const ReverseIterator & _other) const
        {
            return m_it != _other.m_it;
        }

        Iter m_it;
    };
}

#endif //STICK_ITERATOR_HPP
