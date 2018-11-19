#ifndef STICK_PRIVATE_INDEXSEQUENCE_HPP
#define STICK_PRIVATE_INDEXSEQUENCE_HPP

#include <Stick/Platform.hpp>

namespace stick
{
namespace detail
{
template <class T, T... Ints>
struct IntegerSequence
{
};

template <class S>
struct NextIntegerSequence;

template <class T, T... Ints>
struct NextIntegerSequence<IntegerSequence<T, Ints...>>
{
    using type = IntegerSequence<T, Ints..., sizeof...(Ints)>;
};

template <class T, T I, T N>
struct MakeIntSequenceImpl;

template <class T, T N>
using MakeIntegerSequence = typename MakeIntSequenceImpl<T, 0, N>::type;

template <class T, T I, T N>
struct MakeIntSequenceImpl
{
    using type =
        typename NextIntegerSequence<typename MakeIntSequenceImpl<T, I + 1, N>::type>::type;
};

template <class T, T N>
struct MakeIntSequenceImpl<T, N, N>
{
    using type = IntegerSequence<T>;
};

template <Size... Ints>
using IndexSequence = IntegerSequence<Size, Ints...>;

template <Size N>
using MakeIndexSequence = MakeIntegerSequence<Size, N>;
} // namespace detail
} // namespace stick

#endif // STICK_PRIVATE_INDEXSEQUENCE_HPP
