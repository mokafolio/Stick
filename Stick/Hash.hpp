#ifndef STICK_HASH_HPP
#define STICK_HASH_HPP

namespace stick
{
template <class T>
struct DefaultHash;

template <>
struct DefaultHash<Int32>
{
    Size operator()(Int32 _i) const
    {
        return _i;
    }
};

template <>
struct DefaultHash<UInt32>
{
    Size operator()(Int32 _i) const
    {
        return _i;
    }
};

template <>
struct DefaultHash<Size>
{
    Size operator()(Size _i) const
    {
        return _i;
    }
};

template <class T>
struct DefaultHash<T *>
{
    Size operator()(T * _i) const
    {
        return reinterpret_cast<Size>(_i);
    }
};
} // namespace stick

#endif // STICK_HASH_HPP
