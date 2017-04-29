#ifndef STICK_VARIANT_HPP
#define STICK_VARIANT_HPP

#include <Stick/Maybe.hpp>
#include <type_traits>

namespace stick
{
    namespace detail
    {
        template <Size arg1, Size ... others>
        struct StaticMax;

        template <Size arg>
        struct StaticMax<arg>
        {
            static constexpr Size value = arg;
        };

        template <Size arg1, Size arg2, Size ... others>
        struct StaticMax<arg1, arg2, others...>
        {
            static constexpr Size value = arg1 >= arg2 ? StaticMax<arg1, others...>::value :
                                          StaticMax<arg2, others...>::value;
        };

        //@TODO: Make sure this does not conflict with c++14+ compilers
        template< bool B, class T, class F >
        using conditional_t = typename std::conditional<B, T, F>::type;
        template<class...> struct disjunction : std::false_type { };
        template<class B1> struct disjunction<B1> : B1 { };
        template<class B1, class... Bn>
        struct disjunction<B1, Bn...>
        : conditional_t<bool(B1::value), B1, disjunction<Bn...>>  { };

        template<class T, class...Ts>
        struct VariantHelper
        {
            inline static void destroy(TypeID _typeID, void * _storage)
            {
                if (_typeID == TypeInfoT<T>::typeID())
                    reinterpret_cast<T *>(_storage)->~F();
                else
                    VariantHelper<Ts...>::destroy(_typeID, _storage);
            }

            inline static void copy(TypeID _typeID, void * _from, void * _to)
            {
                if (_typeID == TypeInfoT<T>::typeID())
                    new (_to) T(*reinterpret_cast<T *>(_from));
                else
                    VariantHelper<Ts...>::move(_typeID, _from, _to);
            }

            inline static void move(TypeID _typeID, void * _from, void * _to)
            {
                if (_typeID == TypeInfoT<T>::typeID())
                    new (_to) T(std::move(*reinterpret_cast<T *>(_from)));
                else
                    VariantHelper<Ts...>::move(_typeID, _from, _to);
            }
        };

        template<class T, class First, class...Ts>
        struct ConversionValidator
        {
            static constexpr bool valid = std::is_convertible<T, First>::value ? std::is_convertible<T, First>::value : disjunction<std::is_convertible<T, Ts>...>::value;
        };
    }

    template<class...Ts>
    class Variant
    {
        //@TODO: Add static_asserts to make sure the template argument list is not empty,
        // permit reference template arguments etc.

    private:

        using Helper = detail::VariantHelper<Ts...>;

    public:

        static constexpr size_t storageSize = detail::StaticMax<sizeof(Ts)...>::value;
        static constexpr size_t storageAlignment = detail::StaticMax<alignof(Ts)...>::value;
        using StorageType = typename std::aligned_storage<storageSize, storageAlignment>::type;

        inline Variant() :
            m_typeID(0)
        {
        }

        inline Variant(const Variant & _other) :
            m_typeID(_other.m_typeID)
        {
            Helper::copy(_other.m_typeID, &_other.m_storage, &m_storage);
        }

        inline Variant(Variant && _other) :
            m_typeID(_other.m_typeID)
        {
            Helper::move(_other.m_typeID, &_other.m_storage, &m_storage);
        }

        template<class T, class Enable = typename std::enable_if<detail::ConversionValidator<T, Ts...>::valid>::type>
        inline Variant(T && _value)
        {

        }

        inline ~Variant()
        {
            Helper::destroy(m_typeID, &m_storage);
        }

        inline Variant & operator = (const Variant & _other)
        {
            Helper::destroy(m_typeID, &m_storage);
            Helper::copy(_other.m_typeID, &_other.m_storage, &m_storage);
            m_typeID = _other.m_typeID;
            return *this;
        }

        inline Variant & operator = (Variant && _other)
        {
            Helper::destroy(m_typeID, &m_storage);
            Helper::move(_other.m_typeID, &_other.m_storage, &m_storage);
            m_typeID = _other.m_typeID;
            return *this;
        }

        inline bool isValid() const
        {
            return m_typeID != 0;
        }

        template<class T>
        inline bool is() const
        {
            return m_typeID == stick::TypeInfoT<T>::typeID();
        }

        template<class T>
        inline stick::Maybe<T> get() const
        {
            if (m_typeID == stick::TypeInfoT<T>::typeID())
            {
                return *reinterpret_cast<T *>(&m_storage);
            }
            else
            {
                return stick::Maybe<T>();
            }
        }


    private:

        TypeID m_typeID;
        StorageType m_storage;
    };
}

#endif //STICK_VARIANT_HPP
