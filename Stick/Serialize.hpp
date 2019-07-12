#ifndef STICK_SERIALIZER_HPP
#define STICK_SERIALIZER_HPP

#include <Stick/DynamicArray.hpp>
#include <Stick/Error.hpp>
#include <Stick/Utility.hpp>

namespace stick
{

struct STICK_API NoSwap
{
    template <class T, Size B>
    static T swap(T _val)
    {
        return _val;
    }
};

struct STICK_API Swap
{
    static void swapBytes(UInt8 & _v1, UInt8 & _v2)
    {
        UInt8 tmp = _v1;
        _v1 = _v2;
        _v2 = tmp;
    }

    template <class T, Size B>
    static T swap(T _val)
    {
        static_assert(B <= 8 && B > 1, "Attempting to swap a POD type with a strange byte size");
        union
        {
            T val;
            uint8_t c[B];
        };
        val = _val;

        for (Size i = 0; i < B; ++i)
            swapBytes(c[i], c[B - 1 - i]);
        return val;
    }
};

//@TODO: Replace this macro BS with c++ compile time stuff as soon as available (c++20 :)
// store in little endian
struct STICK_API LittleEndianPolicy
{
#ifdef STICK_LITTLE_ENDIAN
    using Swapper = NoSwap;
#else
    using Swapper = Swap;
#endif // STICK_LITTLE_ENDIAN

    template <class T>
    static T convert(T _value)
    {
        return Swapper::swap<T, sizeof(T)>(_value);
    }
};

//@TODO: Big endian policy

template <class T>
struct ContainerWriter
{
    ContainerWriter(T & _target) : target(_target)
    {
    }

    void reserve(Size _count)
    {
        target.reserve(_count);
    }

    void write(const UInt8 * _data, Size _byteCount, UInt32 _align)
    {
        target.append(_data, _data + _byteCount);
        auto padding = (-_byteCount & (_align - 1));
        target.resize(target.count() + padding);
    }

    const UInt8 * dataPtr() const
    {
        if (!target.count())
            return nullptr;
        return &target[0];
    }

    Size position() const
    {
        return target.count();
    }

    Size byteCount() const
    {
        return position();
    }

    T & target;
};

struct MemoryWriter : public ContainerWriter<DynamicArray<UInt8>>
{
    MemoryWriter(Allocator & _alloc = defaultAllocator()) : ContainerWriter(data), data(_alloc)
    {
    }

    DynamicArray<UInt8> data;
};

struct MemoryReader
{
    MemoryReader(const UInt8 * _data, Size _byteCount)
        : start(_data), end(_data + _byteCount), pos(_data)
    {
    }

    template <class T>
    Error readInto(T * _output, UInt32 _align)
    {
        //@TODO: Better error code
        if (end - pos < static_cast<std::ptrdiff_t>(sizeof(T)))
            return Error(ec::InvalidOperation, "Not enough data left", STICK_FILE, STICK_LINE);
        *_output = *((T *)pos);
        pos += sizeof(T) + (-sizeof(T) & (_align - 1));
        return Error();
    }

    template <class T>
    T read(UInt32 _align)
    {
        STICK_ASSERT(end - pos >= static_cast<std::ptrdiff_t>(sizeof(T)));
        T ret = *((T *)pos);
        pos += sizeof(T) + (-sizeof(T) & (_align - 1));
        return ret;
    }

    const char * readCString(UInt32 _align)
    {
        if (end - pos < 2)
            return nullptr;

        const char * ret = reinterpret_cast<const char *>(pos);
        auto len = std::strlen(ret) + 1;
        pos += len + (-len & (_align - 1));
        return ret;
    }

    //@TODO: should there be a version that takes alignment into account?
    void setPosition(Size _byteOffset)
    {
        //@TODO: should this function return an Error instead of asserting?
        pos = start + _byteOffset;
        STICK_ASSERT(pos <= end);
    }

    Size position() const
    {
        return pos - start;
    }

    const UInt8 * start;
    const UInt8 * end;
    const UInt8 * pos;
};

//@TODO: Add support for alignment to the de-/serializer

template <class EP, class SP, UInt32 Align = 4>
class STICK_API SerializerT
{
  public:
    using EndianPolicy = EP;
    using Storage = SP;
    static constexpr UInt32 Alignment = Align;

    template <class... Args>
    SerializerT(Args &&... _args) : m_storage(std::forward<Args>(_args)...)
    {
    }

    Size positionAfterWritingBytes(Size _byteCount)
    {
        //@TODO: should this sit in the storage?
        return m_storage.position() + _byteCount + (_byteCount & (Alignment - 1));
    }

    template <class T>
    void write(T _value)
    {
        T val = EndianPolicy::convert(_value);
        m_storage.write((const UInt8 *)&val, sizeof(T), Alignment);
    }

    void write(const char * _data)
    {
        write(_data, std::strlen(_data) + 1);
    }

    void write(const char * _data, Size _byteCount)
    {
        m_storage.write((const UInt8 *)_data, _byteCount, Alignment);
    }

    void write(const UInt8 * _data, Size _byteCount)
    {
        m_storage.write(_data, _byteCount, Alignment);
    }

    void write(const String & _data)
    {
        if (_data.isEmpty())
            write("");
        else
            write(_data.cString(), _data.length() + 1);
    }

    template <class T>
    void writeComplex(T & _obj)
    {
        _obj.serialize(*this);
    }

    void reserve(Size _byteCount)
    {
        m_storage.reserve(_byteCount);
    }

    const Storage & storage() const
    {
        return m_storage;
    }

    Size position() const
    {
        return m_storage.position();
    }

  private:
    Storage m_storage;
};

template <class EP, class SP, UInt32 Align = 4>
class STICK_API DeserializerT
{
  public:
    using EndianPolicy = EP;
    using Source = SP;
    static constexpr UInt32 Alignment = Align;

    template <class... Args>
    DeserializerT(Args &&... _args) : m_source(std::forward<Args>(_args)...)
    {
    }

    DeserializerT(Source && _source) : m_source(std::move(_source))
    {
    }

    template <class T>
    Error readInto(T * _out)
    {
        T tmp;
        Error err = m_source.readInto(&tmp, Alignment);
        if (err)
            return err;

        *_out = EndianPolicy::convert(tmp);
        return Error();
    }

    Size position() const
    {
        return m_source.position();
    }

    void setPosition(Size _byteOffset)
    {
        m_source.setPosition(_byteOffset);
    }

    bool readBool()
    {
        return m_source.template read<bool>(Alignment);
    }

    Int8 readInt8()
    {
        return m_source.template read<Int8>(Alignment);
    }

    UInt8 readUInt8()
    {
        return m_source.template read<UInt8>(Alignment);
    }

    Int16 readInt16()
    {
        return m_source.template read<Int16>(Alignment);
    }

    UInt16 readUInt16()
    {
        return m_source.template read<UInt16>(Alignment);
    }

    Int32 readInt32()
    {
        return m_source.template read<Int32>(Alignment);
    }

    UInt32 readUInt32()
    {
        return m_source.template read<UInt32>(Alignment);
    }

    Int64 readInt64()
    {
        return m_source.template read<Int64>(Alignment);
    }

    UInt64 readUInt64()
    {
        return m_source.template read<UInt64>(Alignment);
    }

    Float32 readFloat32()
    {
        return m_source.template read<Float32>(Alignment);
    }

    Float64 readFloat64()
    {
        return m_source.template read<Float64>(Alignment);
    }

    const char * readCString()
    {
        return m_source.readCString(Alignment);
    }

  private:
    Source m_source;
};

using MemorySerializerLE = SerializerT<LittleEndianPolicy, MemoryWriter>;
using MemoryDeserializerLE = DeserializerT<LittleEndianPolicy, MemoryReader>;

} // namespace stick

#endif // STICK_SERIALIZER_HPP
