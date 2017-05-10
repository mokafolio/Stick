#ifndef STICK_STICKHASHMAP_HPP
#define STICK_STICKHASHMAP_HPP

#include <Stick/Allocator.hpp>
#include <Stick/String.hpp>
#include <Stick/Detail/MurmurHash2.hpp>
#include <initializer_list>

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

    template<>
    struct DefaultHash<UInt32>
    {
        Size operator()(Int32 _i) const
        {
            return _i;
        }
    };


    template<>
    struct DefaultHash<Size>
    {
        Size operator()(Size _i) const
        {
            return _i;
        }
    };

    template<class T>
    struct DefaultHash<T *>
    {
        Size operator()(T * _i) const
        {
            return reinterpret_cast<Size>(_i);
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

        template<class T>
        struct IterT
        {
            typedef typename T::KeyValuePair ValueType;

            typedef ValueType & ReferenceType;

            typedef ValueType * PointerType;


            IterT() :
                map(nullptr),
                bucketIndex(0),
                node(nullptr)
            {

            }

            IterT(T & _map, Size _bucketIndex, Node * _node) :
                map(&_map),
                bucketIndex(_bucketIndex),
                node(_node)
            {

            }

            inline void increment()
            {
                if (!node)
                    return;

                if (node->next)
                {
                    node = node->next;
                }
                else
                {
                    Node * nn = nullptr;
                    while (!nn && bucketIndex < map->m_bucketCount - 1)
                    {
                        ++bucketIndex;
                        nn = map->m_buckets[bucketIndex].first;
                    }
                    node = nn;
                }
            }

            inline bool operator == (const IterT & _other) const
            {
                return node == _other.node;
            }

            inline bool operator != (const IterT & _other) const
            {
                return node != _other.node;
            }

            inline IterT & operator++()
            {
                increment();
                return *this;
            }

            inline IterT operator++(int)
            {
                IterT ret = *this;
                increment();
                return ret;
            }

            inline IterT & operator+=(Size _i)
            {
                for (Size i = 0; i <= _i; ++i)
                    increment();
                return *this;
            }

            inline IterT operator+(Size _i) const
            {
                IterT ret = *this;
                for (Size i = 0; i <= _i; ++i)
                    ret.increment();
                return ret;
            }

            inline KeyValuePair & operator * () const
            {
                return node->kv;
            }

            inline KeyValuePair  * operator -> () const
            {
                return &node->kv;
            }

        private:

            T * map;
            Size bucketIndex;
            Node * node;
        };

        typedef IterT<HashMap> Iter;
        typedef IterT<const HashMap> ConstIter;

        struct InsertResult
        {
            Iter iterator;
            bool inserted;
        };

        inline HashMap(Size _initialBucketCount = 16, Allocator & _alloc = defaultAllocator()) :
            m_alloc(&_alloc),
            m_buckets(nullptr),
            m_maxLoadFactor(1.0f),
            m_count(0)
        {
            m_buckets = allocateBuckets(_initialBucketCount);
            STICK_ASSERT(m_buckets);
            m_bucketCount = _initialBucketCount;
        }

        inline HashMap(const HashMap & _other) :
            m_alloc(_other.m_alloc),
            m_buckets(nullptr),
            m_bucketCount(0),
            m_maxLoadFactor(_other.m_maxLoadFactor),
            m_count(_other.m_count)
        {
            m_buckets = allocateBuckets(_other.m_bucketCount);
            STICK_ASSERT(m_buckets);
            m_bucketCount = _other.m_bucketCount;
            //iterate over all the buckets and copy each linked list
            for (Size i = 0; i < _other.m_bucketCount; ++i)
            {
                if (_other.m_buckets[i].first)
                    m_buckets[i].first = copyNode(_other.m_buckets[i].first);
            }
        }

        inline HashMap(HashMap && _other) :
            m_alloc(std::move(_other.m_alloc)),
            m_buckets(std::move(_other.m_buckets)),
            m_bucketCount(std::move(_other.m_bucketCount)),
            m_maxLoadFactor(std::move(_other.m_maxLoadFactor)),
            m_count(std::move(_other.m_count))
        {
            _other.m_buckets = nullptr;
        }

        inline HashMap(std::initializer_list<KeyValuePair> _l, Allocator & _alloc = defaultAllocator()) :
            m_alloc(&_alloc),
            m_buckets(nullptr),
            m_bucketCount(0),
            m_maxLoadFactor(1.0f),
            m_count(0)
        {
            m_buckets = allocateBuckets(16);
            m_bucketCount = 16;
            STICK_ASSERT(m_buckets);
            insert(_l);
        }

        inline ~HashMap()
        {
            //deallocate buckets and nodes
            if (m_buckets)
            {
                clear();
                //I guess technically buckets are pod types right now so we
                //don't need to call the destructor...safety first though.
                for (Size i = 0; i < m_bucketCount; ++i)
                    m_buckets[i].~Bucket();

                m_alloc->deallocate({m_buckets, sizeof(Bucket) * m_bucketCount});
            }
        }

        inline HashMap & operator = (const HashMap & _other)
        {
            clear();
            if (m_buckets)
                m_alloc->deallocate({m_buckets, sizeof(Bucket) * m_bucketCount});

            m_alloc = _other.m_alloc;
            m_bucketCount = _other.m_bucketCount;
            m_count = _other.m_count;
            m_maxLoadFactor = _other.m_maxLoadFactor;
            m_buckets = allocateBuckets(m_bucketCount);
            STICK_ASSERT(m_buckets);
            for (Size i = 0; i < _other.m_bucketCount; ++i)
            {
                if (_other.m_buckets[i].first)
                    m_buckets[i].first = copyNode(_other.m_buckets[i].first);
            }
            return *this;
        }

        inline HashMap & operator = (HashMap && _other)
        {
            clear();
            if (m_buckets)
                m_alloc->deallocate({m_buckets, sizeof(Bucket) * m_bucketCount});

            m_alloc = std::move(_other.m_alloc);
            m_bucketCount = std::move(_other.m_bucketCount);
            m_count = std::move(_other.m_count);
            m_maxLoadFactor = std::move(_other.m_maxLoadFactor);
            m_buckets = std::move(_other.m_buckets);

            _other.m_buckets = nullptr;

            return *this;
        }

        inline HashMap & operator = (std::initializer_list<KeyValuePair> _l)
        {
            clear();
            insert(_l);
            return *this;
        }

        inline InsertResult insert(const KeyType & _key, const ValueType & _value)
        {
            return insert((KeyValuePair) {_key, _value});
        }

        inline InsertResult insert(const KeyType & _key, ValueType && _value)
        {
            return insert((KeyValuePair) {_key, std::move(_value)});
        }

        inline InsertResult insert(const KeyValuePair & _val)
        {
            Size bi = bucketIndex(_val.key);
            Bucket & b = m_buckets[bi];

            //check if the key allready exists
            Node * n, *prev;
            findHelper(b, _val.key, n, prev);

            //the key allready exists, change the value
            if (n)
            {
                n->kv.value = _val.value;
                return {Iter(*this, bi, n), false};
            }
            else
            {
                //otherwise create the node n stuff
                n = createNode({_val.key, _val.value});
                if (prev)
                    prev->next = n;
                else
                    b.first = n;
                ++m_count;
            }

            Float32 lf = loadFactor();
            if (lf > m_maxLoadFactor)
            {
                rehash(m_bucketCount * 2);
            }

            return {Iter(*this, bi, n), true};
        }

        inline InsertResult insert(KeyValuePair && _val)
        {
            Size bi = bucketIndex(_val.key);
            Bucket & b = m_buckets[bi];

            //check if the key allready exists
            Node * n, *prev;
            findHelper(b, _val.key, n, prev);

            //the key allready exists, change the value
            if (n)
            {
                n->kv.value = std::move(_val.value);
                return {Iter(*this, bi, n), false};
            }
            else
            {
                //otherwise create the node n stuff
                n = createNode({_val.key, std::move(_val.value)});
                if (prev)
                    prev->next = n;
                else
                    b.first = n;
                ++m_count;
            }

            Float32 lf = loadFactor();
            if (lf > m_maxLoadFactor)
            {
                rehash(m_bucketCount * 2);
            }

            return {Iter(*this, bi, n), true};
        }

        template<class InputIterT>
        inline void insert(InputIterT _begin, InputIterT _end)
        {
            while (_begin != _end)
            {
                insert(*_begin);
                ++_begin;
            }
        }

        inline void insert(std::initializer_list<KeyValuePair> _l)
        {
            insert(_l.begin(), _l.end());
        }

        inline Iter find(const KeyType & _key)
        {
            Size bi = bucketIndex(_key);
            Bucket & b = m_buckets[bi];
            Node * n, *prev;
            findHelper(b, _key, n, prev);
            if (!n)
                return end();
            else
                return Iter(*this, bi, n);
        }

        inline ConstIter find(const KeyType & _key) const
        {
            Size bi = bucketIndex(_key);
            Bucket & b = m_buckets[bi];
            Node * n, *prev;
            findHelper(b, _key, n, prev);
            if (!n)
                return end();
            else
                return ConstIter(*this, bi, n);
        }

        inline ValueType & operator [] (const KeyType & _key)
        {
            auto it = find(_key);
            if (it != end())
            {
                return it->value;
            }
            else
            {
                auto res = insert(_key, ValueType());
                return res.iterator->value;
            }
        }

        inline Iter remove(const KeyType & _key)
        {
            Size bi = bucketIndex(_key);
            Bucket & b = m_buckets[bi];
            Node * n, *prev;
            findHelper(b, _key, n, prev);

            if (!n)
                return end();

            Iter ret;
            if (n->next)
                ret = Iter(*this, bi, n->next);
            else
                ret = end();

            if (!prev)
            {
                b.first = n->next;
            }
            else
            {
                prev->next = n->next;
            }

            destroyNode(n);
            --m_count;
            return ret;
        }

        inline void clear()
        {
            for (Size i = 0; i < m_bucketCount; ++i)
            {
                Bucket & b = m_buckets[i];
                Node * n = b.first;
                while (n)
                {
                    Node * on = n;
                    n = n->next;
                    destroyNode(on);
                }
                b.first = nullptr;
            }
            m_count = 0;
        }

        inline void rehash(Size _bucketCount)
        {
            Bucket * newBuckets = allocateBuckets(_bucketCount);

            for (Size i = 0; i < m_bucketCount; ++i)
            {
                Bucket & b = m_buckets[i];
                Node * n = b.first;

                while (n)
                {
                    Size bucketIndex = m_hasher(n->kv.key) % _bucketCount;
                    Node * nextn = n->next;
                    if (!newBuckets[bucketIndex].first)
                    {
                        newBuckets[bucketIndex].first = n;
                    }
                    else
                    {
                        Node * n2 = newBuckets[bucketIndex].first;
                        while (n2->next)
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

        inline Size count() const
        {
            return m_count;
        }

        inline Float32 maxLoadFactor() const
        {
            return m_maxLoadFactor;
        }

        inline Float32 loadFactor() const
        {
            return (Float32)count() / (Float32)bucketCount();
        }

        inline Iter begin()
        {
            Bucket * b;
            Size i = 0;
            for (; i < m_bucketCount; ++i)
            {
                if (m_buckets[i].first)
                {
                    b = &m_buckets[i];
                    break;
                }
            }
            if (!b)
                return end();
            else
                return Iter(*this, i, b->first);
        }

        inline ConstIter begin() const
        {
            Bucket * b;
            Size i = 0;
            for (; i < m_bucketCount; ++i)
            {
                if (m_buckets[i].first)
                {
                    b = &m_buckets[i];
                    break;
                }
            }
            if (!b)
                return end();
            else
                return ConstIter(*this, i, b->first);
        }

        inline Iter end()
        {
            return Iter();
        }

        inline ConstIter end() const
        {
            return ConstIter();
        }

    private:

        inline Node * createNode(KeyValuePair && _pair)
        {
            auto mem = m_alloc->allocate(sizeof(Node));
            Node * ret = new (mem.ptr) Node;
            ret->kv = std::move(_pair);
            return ret;
        }

        inline void destroyNode(Node * _n)
        {
            _n->~Node();
            m_alloc->deallocate({_n, sizeof(Node)});
        }

        inline Size bucketIndex(const KeyType & _key) const
        {
            return m_hasher(_key) % bucketCount();
        }

        inline void findHelper(Bucket & _bucket, const KeyType & _key, Node *& _outNode, Node *& _prev) const
        {
            _outNode = nullptr;
            Node * n = _bucket.first;
            _prev = nullptr;

            while (n)
            {
                if (_key == n->kv.key)
                {
                    _outNode = n;
                    return;
                }
                _prev = n;
                n = n->next;
            }
        }

        inline Bucket * allocateBuckets(Size _i)
        {
            auto mem = m_alloc->allocate(sizeof(Bucket) * _i);
            return new (mem.ptr) Bucket [_i];
        }

        inline Node * copyNode(Node * _node)
        {
            Node * ret = createNode({_node->kv.key, _node->kv.value});
            if (_node->next)
                ret->next = copyNode(_node->next);
            return ret;
        }

        Allocator * m_alloc;
        Bucket * m_buckets;
        Size m_bucketCount;
        Float32 m_maxLoadFactor;
        Hash m_hasher;
        Size m_count;
    };
}

#endif //STICK_STICKHASHMAP_HPP
