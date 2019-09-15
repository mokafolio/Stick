#ifndef STICK_ITERATOR_HPP
#define STICK_ITERATOR_HPP

#include <Stick/Platform.hpp>
#include <iterator>

namespace stick
{
template <class T>
struct IteratorTraits
{
    typedef typename T::ValueType ValueType;
    typedef typename T::ReferenceType ReferenceType;
    typedef typename T::PointerType PointerType;
};

template <class T>
struct IteratorTraits<T *>
{
    typedef T ValueType;
    typedef T & ReferenceType;
    typedef T * PointerType;
};

template <class T>
struct IteratorTraits<const T *>
{
    typedef const T ValueType;
    typedef const T & ReferenceType;
    typedef const T * PointerType;
};

template <class T>
struct ReverseIterator
{
    typedef IteratorTraits<T> IterTraits;

    typedef T Iter;

    typedef typename IterTraits::ValueType ValueType;

    typedef typename IterTraits::ReferenceType ReferenceType;

    typedef typename IterTraits::PointerType PointerType;

    inline ReverseIterator()
    {
    }

    inline explicit ReverseIterator(const Iter & _it) : m_it(_it)
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

    inline ReferenceType operator*() const
    {
        return *(m_it - 1);
    }

    inline PointerType operator->() const
    {
        return (m_it - 1).operator->();
    }

    inline bool operator==(const ReverseIterator & _other) const
    {
        return m_it == _other.m_it;
    }

    inline bool operator!=(const ReverseIterator & _other) const
    {
        return m_it != _other.m_it;
    }

    Iter m_it;
};

template <class Container>
struct BackInsertIterator
{
    using ContainerType = Container;

    // to satisfy std::iterator
    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;
    using container_type = Container;

    explicit BackInsertIterator(Container & _c) : m_container(_c)
    {
    }

    inline BackInsertIterator & operator++()
    {
        return *this;
    }

    inline BackInsertIterator & operator++(int)
    {
        return *this;
    }

    inline BackInsertIterator & operator*()
    {
        return *this;
    }

    inline BackInsertIterator & operator=(const typename ContainerType::ValueType & _val)
    {
        m_container.append(_val);
        return *this;
    }

    inline BackInsertIterator & operator=(typename ContainerType::ValueType && _val)
    {
        m_container.append(std::move(_val));
        return *this;
    }

    Container & m_container;
};

template <class Container>
BackInsertIterator<Container> backInserter(Container & _c)
{
    return BackInsertIterator<Container>(_c);
}

} // namespace stick

#endif // STICK_ITERATOR_HPP
