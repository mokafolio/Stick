#ifndef STICK_VARIANT_HPP
#define STICK_VARIANT_HPP

#include <Stick/TypeInfo.hpp>
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

        template <typename... Types>
        struct VariantHelper;

        template<class T, class...Ts>
        struct VariantHelper<T, Ts...>
        {

            inline static void destroy(TypeID _typeID, void * _storage)
            {
                if (_typeID == TypeInfoT<T>::typeID())
                {
                    reinterpret_cast<T *>(_storage)->~T();
                }
                else
                    VariantHelper<Ts...>::destroy(_typeID, _storage);
            }

            inline static void copy(TypeID _typeID, const void * _from, void * _to)
            {
                if (_typeID == TypeInfoT<T>::typeID())
                    new (_to) T(*(const T *)(_from));
                else
                    VariantHelper<Ts...>::copy(_typeID, _from, _to);
            }

            inline static void move(TypeID _typeID, void * _from, void * _to)
            {
                if (_typeID == TypeInfoT<T>::typeID())
                    new (_to) T(std::move(*reinterpret_cast<T *>(_from)));
                else
                    VariantHelper<Ts...>::move(_typeID, _from, _to);
            }
        };


        template<class T, class...Ts>
        struct VariantHelper<T &, Ts...>
        {
            using WrappedT = std::reference_wrapper<T>;

            inline static void destroy(TypeID _typeID, void * _storage)
            {
                if (_typeID == TypeInfoT<T &>::typeID())
                {
                    reinterpret_cast<WrappedT *>(_storage)->~WrappedT();
                }
                else
                    VariantHelper<Ts...>::destroy(_typeID, _storage);
            }

            inline static void copy(TypeID _typeID, const void * _from, void * _to)
            {
                if (_typeID == TypeInfoT<T &>::typeID())
                    new (_to) WrappedT(*(const WrappedT *)(_from));
                else
                    VariantHelper<Ts...>::copy(_typeID, _from, _to);
            }

            inline static void move(TypeID _typeID, void * _from, void * _to)
            {
                if (_typeID == TypeInfoT<T &>::typeID())
                    new (_to) WrappedT(std::move(*reinterpret_cast<WrappedT *>(_from)));
                else
                    VariantHelper<Ts...>::move(_typeID, _from, _to);
            }
        };

        template<class T>
        struct StorageAccessor
        {
            static T & access(void * _storage)
            {
                return *reinterpret_cast<T *>(_storage);
            }

            static const T & access(const void * _storage)
            {
                return *reinterpret_cast<const T *>(_storage);
            }
        };

        template<class T>
        struct StorageAccessor<T &>
        {
            using WrappedT = std::reference_wrapper<T>;

            static T & access(void * _storage)
            {
                return reinterpret_cast<WrappedT *>(_storage)->get();
            }

            static const T & access(const void * _storage)
            {
                return reinterpret_cast<const WrappedT *>(_storage)->get();
            }
        };

        template <>
        struct VariantHelper<>
        {
            static void destroy(TypeID, void *) {}
            static void copy(TypeID, const void *, void *) {}
            static void move(TypeID, void *, void *) {}
        };

        template<class T, class...Ts>
        struct DirectType;

        template<class T, class First, class...Ts>
        struct DirectType<T, First, Ts...>
        {
            static constexpr TypeID typeID = std::is_same<T, First>::value ? TypeInfoT<T>::typeID() : DirectType<T, Ts...>::typeID;
        };

        template<class T>
        struct DirectType<T>
        {
            static constexpr TypeID typeID = 0;
        };

        template<class T, class...Ts>
        struct ConvertibleType;

        template<class T, class First, class...Ts>
        struct ConvertibleType<T, First, Ts...>
        {
            using type = typename std::conditional<std::is_convertible<T, First>::value, First, typename ConvertibleType<T, Ts...>::type>::type;
            //@TODO: Add check to make sure that T can only convert to one type (to catch ambigous conversions)
            static constexpr TypeID typeID = std::is_convertible<T, First>::value ? TypeInfoT<First>::typeID() : ConvertibleType<T, Ts...>::typeID;
        };

        template<class T>
        struct ConvertibleType<T>
        {
            using type = T;
            static constexpr TypeID typeID = 0;
        };


        //@TODO: Do we actually need a special traits impl for references?
        template<class T, class...Ts>
        struct Traits
        {
            //@TODO: remove volatile, too?
            using ValueType = typename std::remove_reference<typename std::remove_const<T>::type>::type;
            static constexpr bool bIsDirect = DirectType<T, Ts...>::typeID != 0;
            static constexpr TypeID typeID = bIsDirect ? DirectType<ValueType, Ts...>::typeID : ConvertibleType<ValueType, Ts...>::typeID;
            static constexpr bool bIsValid = typeID != 0;
            using TargetType = typename std::conditional<bIsDirect, ValueType, typename ConvertibleType<ValueType, Ts...>::type>::type;
        };

        template<class T, class...Ts>
        struct Traits<T &, Ts...>
        {
            //@TODO: remove volatile, too?
            using ValueType = typename std::remove_reference<typename std::remove_const<T &>::type>::type;
            static constexpr bool bIsDirect = DirectType<T &, Ts...>::typeID != 0;
            static constexpr TypeID typeID = bIsDirect ? DirectType<T &, Ts...>::typeID : ConvertibleType<T &, Ts...>::typeID;
            static constexpr bool bIsValid = typeID != 0;
            //@TODO: ConvertibleType should most likely differ between ref/no ref aswell, so that implicit conversions
            //between references can become possible with the correct storage.
            using TargetType = typename std::conditional<bIsDirect, std::reference_wrapper<ValueType>, typename ConvertibleType<T &, Ts...>::type>::type;
        };

        template<class T>
        struct ReturnTypeTraits
        {
            //@TODO: remove volatile, too?
            using ValueType = typename std::remove_reference<typename std::remove_const<T &>::type>::type;
            using ReferenceType = ValueType&;
            using ConstReferenceType = const ValueType&;
        };
    }

    template<class...Ts>
    class Variant
    {
        //@TODO: Add static_asserts to make sure the template argument list is not empty etc.

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
            _other.m_typeID = 0;
        }

        template<class T, class Enable = typename std::enable_if<detail::Traits<T, Ts...>::bIsValid>::type>
        inline Variant(T && _value)
        {
            using Traits = detail::Traits<T, Ts...>;
            using TT = typename detail::Traits<T, Ts...>::TargetType;
            m_typeID = Traits::typeID;
            new (&m_storage) TT(std::forward<T>(_value));
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
            return m_typeID == TypeInfoT<T>::typeID();
        }

        template<class T>
        inline Maybe<typename detail::ReturnTypeTraits<T>::ConstReferenceType> maybe() const
        {
            if (m_typeID == TypeInfoT<T>::typeID())
            {
                return detail::StorageAccessor<T>::access(&m_storage);
                // return *reinterpret_cast<const T *>(&m_storage);
            }
            else
            {
                return Maybe<const typename detail::ReturnTypeTraits<T>::ReferenceType>();
            }
        }

        template<class T>
        inline Maybe<typename detail::ReturnTypeTraits<T>::ReferenceType> maybe()
        {
            if (m_typeID == TypeInfoT<T>::typeID())
            {
                return detail::StorageAccessor<T>::access(&m_storage);
                // return *reinterpret_cast<T *>(&m_storage);
            }
            else
            {
                return Maybe<typename detail::ReturnTypeTraits<T>::ReferenceType>();
            }
        }

        template<class T>
        inline typename detail::ReturnTypeTraits<T>::ConstReferenceType get() const
        {
            return detail::StorageAccessor<T>::access(&m_storage);
            // return *reinterpret_cast<const typename detail::ReturnTypeTraits<T>::ValueType*>(&m_storage);
        }

        template<class T>
        inline typename detail::ReturnTypeTraits<T>::ReferenceType get()
        {
            return const_cast<typename detail::ReturnTypeTraits<T>::ReferenceType>(const_cast<const Variant *>(this)->get<T>());
        }

    private:

        TypeID m_typeID;
        StorageType m_storage;
    };
}

#endif //STICK_VARIANT_HPP
