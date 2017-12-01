#ifndef STICK_TYPELIST_HPP
#define STICK_TYPELIST_HPP

#include <tuple>
#include <type_traits>
#include <Stick/Platform.hpp>

namespace stick
{
    template<class H, class T>
    struct TypeList
    {
        using Head = H;
        using Tail = T;
        static constexpr Size count = Tail::count + 1;
    };

    struct TypeListNil
    {
        static constexpr Size count = 0;
    };

    namespace detail
    {
        template<class L, class T>
        struct HasTypeHelper
        {
            static constexpr bool value = std::is_same<typename L::Head, T>::value ? true : HasTypeHelper<typename L::Tail, T>::value;
        };

        template<class T>
        struct HasTypeHelper<TypeListNil, T>
        {
            static constexpr bool value = false;
        };
    }

    template<class L, class T>
    struct HasType
    {
        static constexpr bool value = detail::HasTypeHelper<L, T>::value;
    };

    namespace detail
    {
        template<class L, Size IDX>
        struct IndexComparator
        {
            static constexpr bool value = L::count - 1 == IDX;
            using Head = typename L::Head;
            using Tail = typename L::Tail;
        };

        template<Size IDX>
        struct IndexComparator<TypeListNil, IDX>
        {
            static constexpr bool value = false;
            using Head = TypeListNil;
            using Tail = TypeListNil;
        };

        template<class L, Size IDX>
        struct TypeAtHelper
        {
            using Comperator = IndexComparator<L, IDX>;
            using Type = typename std::conditional<Comperator::value, typename Comperator::Head, typename TypeAtHelper<typename Comperator::Tail, IDX>::Type>::type;
        };

        template<Size IDX>
        struct TypeAtHelper<TypeListNil, IDX>
        {
            using Type = TypeListNil;
        };
    }

    template<class L, Size IDX>
    struct TypeAt
    {
        using Type = typename detail::TypeAtHelper < L, L::count - 1 - IDX >::Type;
    };

    template<class...Args>
    struct MakeTypeList;

    template<class Head, class...Tail>
    struct MakeTypeList<Head, Tail...>
    {
        using List = TypeList<Head, typename MakeTypeList<Tail...>::List>;
    };

    template<>
    struct MakeTypeList<>
    {
        using List = TypeListNil;
    };

    template<class L, class T>
    struct PrependType
    {
        using List = TypeList<T, L>;
    };

    template<class L, class T>
    struct AppendType
    {
        using List = TypeList<typename L::Head, typename AppendType<typename L::Tail, T>::List>;
    };

    template<class T>
    struct AppendType<TypeListNil, T>
    {
        using List = TypeList<T, TypeListNil>;
    };

    namespace detail
    {
        template<class L, class H, class T>
        struct AppendTypeListHelper
        {
            using List = TypeList<typename L::Head, typename AppendTypeListHelper<typename L::Tail, H, T>::List>;
        };

        template<class H, class T>
        struct AppendTypeListHelper<TypeListNil, H, T>
        {
            using List = TypeList<H, T>;
        };
    }

    template<class L, class L2>
    struct AppendTypeList
    {
        using List = typename detail::AppendTypeListHelper<L, typename L2::Head, typename L2::Tail>::List;
    };

    template<class L>
    struct AppendTypeList<L, TypeListNil>
    {
        using List = L;
    };
}

#endif //STICK_TYPELIST_HPP
