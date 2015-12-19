#ifndef STICK_STICKHASHMAP_HPP
#define STICK_STICKHASHMAP_HPP

#include <Stick/StickDynamicArray.hpp>

namespace stick
{
    template<class K, class V, class H>
    class HashMap
    {
    public:

        typedef K KeyType;
        typedef V ValueType
        typedef H Hash;

    private:

        typedef DynamicArray<ValueType> ValueArray;
        typedef DynamicArray<ValueArray> BucketArray;

        BucketArray m_buckets;
    };
}

#endif //STICK_STICKHASHMAP_HPP
