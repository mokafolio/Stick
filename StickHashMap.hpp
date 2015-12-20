#ifndef STICK_STICKHASHMAP_HPP
#define STICK_STICKHASHMAP_HPP

#include <Stick/StickAllocator.hpp>
#include <Stick/StickString.hpp>
#include <Stick/Detail/StickMurmurHash2.hpp>

#include <iostream>
#include <math.h>

namespace stick
{
    template<class T>
    struct DefaultHash;

    template<>
    struct DefaultHash<String>
    {
        Size operator()(const String & _str) const
        {
            return detail::murmur2(_str.cString(), _str.length(), 0);
        }
    };

    template<>
    struct DefaultHash<Int32>
    {
        Size operator()(Int32 _i) const
        {
            return _i;
        }
    };

    template<class K, class V, template<class> class H = DefaultHash>
    class HashMap
    {
    public:

        typedef K KeyType;
        typedef V ValueType;
        typedef H<KeyType> Hash;

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
            first(nullptr)
            {

            }

            Node * first;
        };

        struct Iter
        {
            typedef KeyValuePair ValueType;

            typedef ValueType & ReferenceType;

            typedef ValueType * PointerType;

            Iter() :
            map(nullptr),
            bucketIndex(0),
            node(nullptr)
            {

            }

            Iter(HashMap & _map, Size _bucketIndex, Node * _node) :
            map(&_map),
            bucketIndex(_bucketIndex),
            node(_node)
            {

            }

            inline void increment()
            {
                if(!node)
                    return;

                if(node->next)
                    node = node->next;
                else
                {
                    if(bucketIndex < map->m_bucketCount)
                    {

                    }
                }
            }

            inline void decrement()
            {

            }

        private:

            HashMap * map;
            Size bucketIndex;
            Node * node;
        };

        HashMap(Size _initialBucketCount = 16, Allocator & _alloc = defaultAllocator()) :
        m_alloc(&_alloc),
        m_buckets(nullptr),
        m_bucketCount(_initialBucketCount),
        m_maxLoadFactor(1.0f),
        m_elementCount(0)
        {  
            auto mem = m_alloc->allocate(sizeof(Bucket) * _initialBucketCount);
            m_buckets = new (mem.ptr) Bucket [_initialBucketCount];
        }

        ~HashMap()
        {
            //deallocate buckets and nodes
            if(m_buckets)
            {
                clear();
                //I guess technically buckets are pod types right now so we
                //don't need to call the destructor...safety first though.
                for(Size i=0; i<m_bucketCount; ++i)
                    m_buckets[i].~Bucket();

                m_alloc->deallocate({m_buckets, sizeof(Bucket) * m_bucketCount});
            }
        }

        inline void insert(const KeyType & _key, const ValueType & _value)
        {   
            Bucket & b = bucket(_key);

            //check if the key allready exists
            Node * n, *prev;
            findHelper(b, _key, n, prev);

            //the key allready exists, change the value
            if(n)
            {
                n->kv.value = _value;
            }
            else
            {
                //otherwise create the node n stuff
                n = createNode(_key, _value);
                if(prev)
                    prev->next = n;
                else
                    b.first = n;
                ++m_elementCount;
            }

            Float32 lf = loadFactor();
            if(lf > m_maxLoadFactor)
            { 
                rehash(m_bucketCount * 2);
            }
        }

        inline void remove(const KeyType & _key)
        {
            Bucket & b = bucket(_key);
            Node * n, *prev;
            findHelper(b, _key, n, prev);

            if(!n)
                return;

            if(!prev)
            {
                b.first = n->next;
            }
            else
            {
                prev->next = n->next;
            }

            destroyNode(n);
            --m_elementCount;
        }

        inline void clear()
        {
            for(Size i=0; i<m_bucketCount; ++i)
            {
                Bucket & b = m_buckets[i];
                Node * n = b.first;
                while(n)
                {
                    Node * on = n;
                    n = n->next;
                    destroyNode(on);
                }
                b.first = nullptr;
            }
        }

        inline void rehash(Size _bucketCount)
        {
            auto mem = m_alloc->allocate(sizeof(Bucket) * _bucketCount);
            Bucket * newBuckets = new (mem.ptr) Bucket[_bucketCount];

            for(Size i=0; i<m_bucketCount; ++i)
            {
                Bucket & b = m_buckets[i];
                Node * n = b.first;

                while(n)
                {
                    Size bucketIndex = m_hasher(n->kv.key) % _bucketCount;
                    Node * nextn = n->next;
                    if(!newBuckets[bucketIndex].first)
                    {
                        newBuckets[bucketIndex].first = n;
                    }
                    else
                    {
                        Node * n2 = newBuckets[bucketIndex].first;
                        while(n2->next)
                            n2 = n2->next;
                        n2->next = n;
                    }
                    n->next = nullptr;
                    n = nextn;
                }

                b.~Bucket();
            }

            m_alloc->deallocate({m_buckets, sizeof(Bucket) * m_bucketCount});
            m_buckets = newBuckets;
            m_bucketCount = _bucketCount;
        }

        inline Size bucketCount() const
        {
            return m_bucketCount;
        }

        inline Size elementCount() const
        {
            return m_elementCount;
        }

        inline Float32 maxLoadFactor() const
        {
            return m_maxLoadFactor;
        }

        inline Float32 loadFactor() const
        {
            return (Float32)elementCount() / (Float32)bucketCount();
        }

    private:

        inline Node * createNode(const KeyType & _key, const ValueType & _val)
        {
            auto mem = m_alloc->allocate(sizeof(Node));
            Node * ret = new (mem.ptr) Node;
            ret->kv = {_key, _val};
            return ret;
        }

        inline void destroyNode(Node * _n)
        {
            _n->~Node();
            m_alloc->deallocate({_n, sizeof(Node)});
        }

        inline Bucket & bucket(const KeyType & _key)
        {
            Size bucketIndex = m_hasher(_key) % bucketCount();
            return m_buckets[bucketIndex];
        }

        inline void findHelper(Bucket & _bucket, const KeyType & _key, Node *& _outNode, Node *& _prev)
        {
            _outNode = nullptr;
            Node * n = _bucket.first;
            _prev = nullptr;

            while(n)
            {
                if(_key == n->kv.key)
                {
                    _outNode = n;
                    return;
                }
                _prev = n;
                n = n->next;
            }
        }

        Allocator * m_alloc;
        Bucket * m_buckets;
        Size m_bucketCount;
        Float32 m_maxLoadFactor;
        Hash m_hasher;
        Size m_elementCount;
    };
}

#endif //STICK_STICKHASHMAP_HPP
