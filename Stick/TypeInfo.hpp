#ifndef STICK_TYPEINFO_HPP
#define STICK_TYPEINFO_HPP

#include <Stick/Platform.hpp>

namespace stick
{
    /**
    * @brief An integer representing a type id.
    */
    typedef void* TypeID;

    /**
     * @brief A TypeInfo is a utility class to get a TypeID of type T and to compare
     * different types.
     */
    template<class T>
    class STICK_API TypeInfoT
    {
    public:

        typedef T Type;

        static constexpr char s_helper = 0;

        /**
         * @brief Default Constructor.
         */
        TypeInfoT()
        {

        }

        /**
         * @brief You can derive from this.
         */
        virtual ~TypeInfoT()
        {

        }

        //Do these even make sense? Another O than T will always yield false...
        template<class O>
        bool operator == (const TypeInfoT<O> _other)
        {
            return typeID() == _other.typeID();
        }

        template<class O>
        bool operator != (const TypeInfoT<O> _other)
        {
            return !*this == _other;
        }

        /**
         * @brief Returns the TypeID of T.
         */
        static constexpr TypeID typeID()
        {
            return (TypeID)&s_helper;
        }
    };

    template<class T>
    constexpr char TypeInfoT<T>::s_helper;

    namespace detail
    {
        STICK_API TypeID __typeIDInt32SharedLibBoundsTestFunction();
    }
}

#endif //STICK_TYPEINFO_HPP
