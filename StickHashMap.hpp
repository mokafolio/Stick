#ifndef STICK_STICKHASHMAP_HPP
#define STICK_STICKHASHMAP_HPP

#include <Stick/StickAllocator.hpp>

namespace stick
{
    template<class K, class V, class H>
    class HashMap
    {
    public:

        typedef K KeyType;
        typedef V ValueType;
        typedef H Hash;

        struct KeyValuePair
        {
            KeyType key;
            ValueType value;
        };

        struct Node
        {
            Node() :
            next(nullptr)
            {

            }

            KeyValuePair kv;
            Node * next;
        };

        struct Bucket
        {
            Bucket() :
            elementCount(0)
            {

            }

            Size elementCount;
            Node * first;
        };

        HashMap(Allocator & _alloc = defaultAllocator(), Size _initialBucketCount = 16) :
        m_alloc(&_alloc),
        m_buckets(nullptr),
        m_bucketCount(_initialBucketCount),
        m_maxLoadFactor(1.0f)
        {  
            auto mem = m_alloc->allocate(sizeof(Bucket) * _initialBucketCount);
            m_buckets = new (mem.ptr) Bucket [_initialBucketCount];
        }

        ~HashMap()
        {

        }

        inline void insert(const KeyType & _key, const ValueType & _value)
        {   
            Size bucketIndex = m_hasher(_key) % bucketCount();
            Bucket & b = m_buckets[bucketIndex];

            //check if the key allready exists
            Node * n = b.first;
            Node * prev;
            while(n && n->kv.key != _key)
            {
                prev = n;
                n = n->next;
            }

            //the key allready exists, change the value
            if(n)
                n->kv.value = _value;
            else
            {
                //otherwise create the node n stuff
                n = createNode(_key, _value);
                if(prev)
                {
                    prev->next = n;
                }
                else
                {
                    b.first = n;
                }
            }
        }

        inline void remove(const KeyType & _key)
        {

        }

        inline void rehash(Size _bucketCount)
        {

        }

        inline Size bucketCount() const
        {
            return m_bucketCount;
        }

        inline Size elementCount() const
        {

        }

        inline Float32 maxLoadFactor() const
        {
            return m_maxLoadFactor;
        }

        inline Float32 loadFactor() const
        {
            return elementCount() / bucketCount();
        }

    private:

        inline Node * createNode(const KeyType & _key, const ValueType & _val)
        {
            auto mem = m_alloc->allocate(sizeof(Node));
            Node * ret = new (mem.ptr) Node;
            ret->kv = {_key, _val};
            return ret;
        }

        Allocator * m_alloc;
        Bucket * m_buckets;
        Size m_bucketCount;
        Float32 m_maxLoadFactor;
        Hash m_hasher;
    };
}

#endif //STICK_STICKHASHMAP_HPP
