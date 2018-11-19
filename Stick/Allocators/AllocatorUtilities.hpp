#ifndef STICK_ALLOCATORS_ALLOCATORUTILITIES_HPP
#define STICK_ALLOCATORS_ALLOCATORUTILITIES_HPP

#include <Stick/Platform.hpp>
#include <utility> //for std::declval

namespace stick
{
namespace mem
{
inline Size roundToAlignment(Size _s, Size _alignment)
{
    return _s + ((_s % _alignment == 0) ? 0 : (_alignment - _s % _alignment));
}

inline Size alignmentAdjustment(const void * _ptr, Size _alignment)
{
    Size adjustment =
        _alignment - (reinterpret_cast<UPtr>(_ptr) & static_cast<UPtr>(_alignment - 1));

    if (adjustment == _alignment)
        return 0; // already aligned

    return adjustment;
}

inline Size alignmentAdjustmentWithHeader(const void * _ptr, Size _alignment, Size _headerSize)
{
    Size adjustment = alignmentAdjustment(_ptr, _alignment);

    if (adjustment < _headerSize)
    {
        _headerSize -= adjustment;

        // Increase adjustment to fit header
        adjustment += _alignment * (_headerSize / _alignment);

        if (_headerSize % _alignment > 0)
            adjustment += _alignment;
    }
    return adjustment;
}

// template <class A, class B = void>
// struct HasOwns : std::false_type
// {
//     using type = bool;
// };

// template <class T>
// struct HasOwns<T, decltype(std::declval<T>().owns())> : std::true_type
// {
//     using type = bool;
// };
} // namespace mem
} // namespace stick

#endif // STICK_ALLOCATORS_ALLOCATORUTILITIES_HPP
