#ifndef STICK_DEFAULTCLEANUP_HPP
#define STICK_DEFAULTCLEANUP_HPP

#include <Stick/Allocator.hpp>

namespace stick
{
template <class T>
class DefaultCleanup
{
  public:
    inline DefaultCleanup() : allocator(nullptr)
    {
    }

    inline DefaultCleanup(Allocator & _alloc) : allocator(&_alloc)
    {
    }

    template <class U>
    inline DefaultCleanup(const DefaultCleanup<U> & _other) : allocator(_other.allocator)
    {
    }

    template <class U>
    inline DefaultCleanup(DefaultCleanup<U> && _other) : allocator(std::move(_other.allocator))
    {
    }

    inline void operator()(T * _obj) const
    {
        allocator->destroy(_obj);
    }

    Allocator * allocator;
};
} // namespace stick

#endif // STICK_DEFAULTCLEANUP_HPP
