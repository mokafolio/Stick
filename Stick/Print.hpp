#ifndef STICK_STICKPRINT_HPP
#define STICK_STICKPRINT_HPP

namespace stick
{
namespace detail
{
template <class T>
struct FmtGenerator;

template <>
struct FmtGenerator<Float32>
{
    static constexpr char fmt[] = "%f ";
};

constexpr char FmtGenerator<Float32>::fmt[4];

template <>
struct FmtGenerator<Float64>
{
    static constexpr char fmt[] = "%f ";
};

constexpr char FmtGenerator<Float64>::fmt[4];

template <>
struct FmtGenerator<Int8>
{
    static constexpr char fmt[] = "%i ";
};

constexpr char FmtGenerator<Int8>::fmt[4];

template <>
struct FmtGenerator<UInt8>
{
    static constexpr char fmt[] = "%u ";
};

constexpr char FmtGenerator<UInt8>::fmt[4];

template <>
struct FmtGenerator<Int16>
{
    static constexpr char fmt[] = "%i ";
};

constexpr char FmtGenerator<Int16>::fmt[4];

template <>
struct FmtGenerator<UInt16>
{
    static constexpr char fmt[] = "%u ";
};

constexpr char FmtGenerator<UInt16>::fmt[4];

template <>
struct FmtGenerator<Int32>
{
    static constexpr char fmt[] = "%i ";
};

constexpr char FmtGenerator<Int32>::fmt[4];

template <>
struct FmtGenerator<UInt32>
{
    static constexpr char fmt[] = "%u ";
};

constexpr char FmtGenerator<UInt32>::fmt[4];

template <>
struct FmtGenerator<Int64>
{
    static constexpr char fmt[] = "%i ";
};

constexpr char FmtGenerator<Int64>::fmt[4];

template <>
struct FmtGenerator<UInt64>
{
    static constexpr char fmt[] = "%u ";
};

constexpr char FmtGenerator<UInt64>::fmt[4];

template <>
struct FmtGenerator<const char *>
{
    static constexpr char fmt[] = "%s ";
};

constexpr char FmtGenerator<const char *>::fmt[4];

template <>
struct FmtGenerator<String>
{
    static constexpr char fmt[] = "%s ";
};

constexpr char FmtGenerator<String>::fmt[4];

template <class T>
struct FmtGenerator<T *>
{
    static constexpr char fmt[] = "%p ";
};

template <class T>
constexpr char FmtGenerator<T *>::fmt[4];

template <class... T>
struct AccumulatedFmt
{
    static char * fmtStr()
    {
        static char s_c[sizeof...(T) * 4];
        memset(s_c, 0, sizeof(s_c));
        int unpack[]{ 0, (strcat(s_c, FmtGenerator<T>::fmt), 0)... };
        return s_c;
    }
};

template <class T>
T forwardToPrint(T _v)
{
    return _v;
}

const char * forwardToPrint(const String & _str)
{
    return _str.cString();
}
} // namespace detail

template <class... T>
inline void print(T... _args)
{
    printf(detail::AccumulatedFmt<T...>::fmtStr(), detail::forwardToPrint(_args)...);
}
} // namespace stick

#endif // STICK_STICKPRINT_HPP
