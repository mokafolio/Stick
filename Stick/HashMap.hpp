#ifndef STICK_HASHMAP_HPP
#define STICK_HASHMAP_HPP

#include <Stick/Allocator.hpp>
#include <Stick/Hash.hpp>
#include <initializer_list>

namespace stick
{
//@TODO Possibly redesign this to not allocate every key value pair independently
// as described here: http://www.ilikebigbits.com/blog/2016/8/28/designing-a-fast-hash-table
template <class K, class V, template <class> class H = DefaultHash>
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
        Node() : prev(nullptr), next(nullptr), bucketIndex(-1), id(-1)
        {
        }

        KeyValuePair kv;
        Node * prev;
        Node * next;
        Size bucketIndex;
        Size id;
    };

    struct Bucket
    {
        Bucket() : first(nullptr)
        {
        }

        Node * first;
    };

    // struct Handle
    // {
    //     Size bucketIndex;
    //     Size id;
    // };

    template <class T>
    struct IterT
    {
        typedef typename T::KeyValuePair ValueType;

        typedef ValueType & ReferenceType;

        typedef ValueType * PointerType;

        IterT() : map(nullptr), bucketIndex(-1), node(nullptr)
        {
        }

        template <class B>
        IterT(const IterT<B> & _other) :
            map(_other.map),
            bucketIndex(_other.bucketIndex),
            node(_other.node)
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

        inline bool operator==(const IterT & _other) const
        {
            return node == _other.node;
        }

        inline bool operator!=(const IterT & _other) const
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

        inline KeyValuePair & operator*() const
        {
            return node->kv;
        }

        inline KeyValuePair * operator->() const
        {
            return &node->kv;
        }

        // inline Handle handle() const
        // {
        //     return {bucketIndex, node ? node->id : -1};
        // }

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
        m_bucketAllocationSize(0),
        m_maxLoadFactor(1.0f),
        m_count(0),
        m_nextNodeID(0)
    {
        m_buckets = allocateBuckets(_initialBucketCount, &m_bucketAllocationSize);
        STICK_ASSERT(m_buckets);
        m_bucketCount = _initialBucketCount;
    }

    inline HashMap(const HashMap & _other) :
        m_alloc(_other.m_alloc),
        m_buckets(nullptr),
        m_bucketAllocationSize(0),
        m_bucketCount(0),
        m_maxLoadFactor(_other.m_maxLoadFactor),
        m_count(_other.m_count),
        m_nextNodeID(_other.m_nextNodeID)
    {
        m_buckets = allocateBuckets(_other.m_bucketCount, &m_bucketAllocationSize);
        STICK_ASSERT(m_buckets);
        m_bucketCount = _other.m_bucketCount;
        // iterate over all the buckets and copy each linked list
        for (Size i = 0; i < _other.m_bucketCount; ++i)
        {
            if (_other.m_buckets[i].first)
                m_buckets[i].first = copyNode(_other.m_buckets[i].first);
        }
    }

    inline HashMap(HashMap && _other) :
        m_alloc(std::move(_other.m_alloc)),
        m_buckets(std::move(_other.m_buckets)),
        m_bucketAllocationSize(std::move(_other.m_bucketAllocationSize)),
        m_bucketCount(std::move(_other.m_bucketCount)),
        m_maxLoadFactor(std::move(_other.m_maxLoadFactor)),
        m_count(std::move(_other.m_count)),
        m_nextNodeID(std::move(_other.m_nextNodeID))
    {
        _other.m_buckets = nullptr;
    }

    inline HashMap(std::initializer_list<KeyValuePair> _l,
                   Allocator & _alloc = defaultAllocator()) :
        m_alloc(&_alloc),
        m_buckets(nullptr),
        m_bucketAllocationSize(0),
        m_bucketCount(0),
        m_maxLoadFactor(1.0f),
        m_count(0),
        m_nextNodeID(0)
    {
        m_buckets = allocateBuckets(16, &m_bucketAllocationSize);
        m_bucketCount = 16;
        STICK_ASSERT(m_buckets);
        insert(_l);
    }

    inline ~HashMap()
    {
        // deallocate buckets and nodes
        if (m_buckets)
        {
            clear();

            // I guess technically buckets are pod types right now so we
            // don't need to call the destructor...safety first though.
            for (Size i = 0; i < m_bucketCount; ++i)
                m_buckets[i].~Bucket();

            m_alloc->deallocate({ m_buckets, m_bucketAllocationSize });
        }
    }

    inline HashMap & operator=(const HashMap & _other)
    {
        clear();
        if (m_buckets)
            m_alloc->deallocate({ m_buckets, m_bucketAllocationSize });

        m_alloc = _other.m_alloc;
        m_bucketCount = _other.m_bucketCount;
        m_bucketAllocationSize = _other.m_bucketAllocationSize;
        m_count = _other.m_count;
        m_maxLoadFactor = _other.m_maxLoadFactor;
        m_buckets = allocateBuckets(m_bucketCount, &m_bucketAllocationSize);
        STICK_ASSERT(m_buckets);
        for (Size i = 0; i < _other.m_bucketCount; ++i)
        {
            if (_other.m_buckets[i].first)
                m_buckets[i].first = copyNode(_other.m_buckets[i].first);
        }
        return *this;
    }

    inline HashMap & operator=(HashMap && _other)
    {
        clear();
        if (m_buckets)
            m_alloc->deallocate({ m_buckets, m_bucketAllocationSize });

        m_alloc = std::move(_other.m_alloc);
        m_bucketCount = std::move(_other.m_bucketCount);
        m_count = std::move(_other.m_count);
        m_maxLoadFactor = std::move(_other.m_maxLoadFactor);
        m_buckets = std::move(_other.m_buckets);
        m_bucketAllocationSize = std::move(_other.m_bucketAllocationSize);

        _other.m_buckets = nullptr;

        return *this;
    }

    inline HashMap & operator=(std::initializer_list<KeyValuePair> _l)
    {
        clear();
        insert(_l);
        return *this;
    }

    inline InsertResult insert(const KeyType & _key, const ValueType & _value)
    {
        return insert((KeyValuePair){ _key, _value });
    }

    inline InsertResult insert(const KeyType & _key, ValueType && _value)
    {
        return insert((KeyValuePair){ _key, std::move(_value) });
    }

    inline InsertResult insert(const KeyValuePair & _val)
    {
        Size bi = bucketIndex(_val.key);
        Bucket & b = m_buckets[bi];

        // check if the key allready exists
        Node *n, *prev;
        findHelper(bi, _val.key, n, prev);

        // the key allready exists, change the value
        if (n)
        {
            n->kv.value = _val.value;
            return { Iter(*this, bi, n), false };
        }
        else
        {
            // otherwise create the node n stuff
            n = createNode({ _val.key, _val.value }, bi);
            if (prev)
            {
                n->prev = prev;
                prev->next = n;
            }
            else
                b.first = n;
            ++m_count;
        }

        Float32 lf = loadFactor();
        if (lf > m_maxLoadFactor)
        {
            rehash(m_bucketCount * 2);
        }

        return { Iter(*this, bi, n), true };
    }

    inline InsertResult insert(KeyValuePair && _val)
    {
        Size bi = bucketIndex(_val.key);
        Bucket & b = m_buckets[bi];

        // check if the key allready exists
        Node *n, *prev;
        findHelper(bi, _val.key, n, prev);

        // the key allready exists, change the value
        if (n)
        {
            STICK_ASSERT(prev == n->prev);
            n->kv.value = std::move(_val.value);
            return { Iter(*this, bi, n), false };
        }
        else
        {
            // otherwise create the node n stuff
            n = createNode({ _val.key, std::move(_val.value) }, bi);
            if (prev)
            {
                n->prev = prev;
                prev->next = n;
            }
            else
            {
                b.first = n;
            }
            ++m_count;
        }

        Float32 lf = loadFactor();
        if (lf > m_maxLoadFactor)
        {
            rehash(m_bucketCount * 2);
        }

        return { Iter(*this, bi, n), true };
    }

    template <class InputIterT>
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
        return HashMap::findByKey<Iter>(*this, _key);
    }

    inline ConstIter find(const KeyType & _key) const
    {
        return HashMap::findByKey<ConstIter>(*this, _key);
    }

    // inline Iter find(const Handle & _handle)
    // {
    //     return HashMap::findByHandle<Iter>(*this, _handle);
    // }

    // inline ConstIter find(const Handle & _handle) const
    // {
    //     return HashMap::findByHandle<ConstIter>(*this, _handle);
    // }

    //@TODO: Add a KeyType && _key overloaded version of it?
    inline ValueType & operator[](const KeyType & _key)
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
        auto it = find(_key);
        return remove(it);
    }

    inline Iter remove(ConstIter _it)
    {
        if (!_it.node)
            return Iter();

        Bucket & b = m_buckets[_it.bucketIndex];

        //@TODO: Still not 100% positive that this is correct
        // and its also kind of ugly.
        Iter ret(*this, _it.bucketIndex, _it.node);
        ++ret;
        // if (_it.node->next)
        // {
        //     ret = Iter(*this, _it.node->next->bucketIndex, _it.node->next);
        // }
        // else if(_it.bucketIndex < m_bucketCount - 1)
        // {
        //     ret = Iter(*this, _it.)
        // }
        // else
        //     ret = end();

        if (!_it.node->prev)
            b.first = _it.node->next;
        else
            _it.node->prev->next = _it.node->next;

        if (_it.node->next)
            _it.node->next->prev = _it.node->prev;

        destroyNode(_it.node);
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
        Size newBucketAllocationSize = 0;
        Bucket * newBuckets = allocateBuckets(_bucketCount, &newBucketAllocationSize);

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
                    n->prev = nullptr;
                }
                else
                {
                    Node * n2 = newBuckets[bucketIndex].first;
                    while (n2->next)
                        n2 = n2->next;
                    n2->next = n;
                    n->prev = n2;
                }
                n->next = nullptr;
                n = nextn;
            }

            b.~Bucket();
        }

        m_alloc->deallocate({ m_buckets, m_bucketAllocationSize });
        m_bucketAllocationSize = newBucketAllocationSize;
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
        Bucket * b = nullptr;
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

    Allocator & allocator() const
    {
        return *m_alloc;
    }

    inline Size bucketIndex(const KeyType & _key) const
    {
        return m_hasher(_key) % bucketCount();
    }

  private:
    inline Node * createNode(KeyValuePair && _pair, Size _bucketIndex)
    {
        auto ret = m_alloc->create<Node>();
        ret->bucketIndex = _bucketIndex;
        ret->id = m_nextNodeID++;
        ret->kv = std::move(_pair);
        return ret;
    }

    inline void destroyNode(Node * _n)
    {
        m_alloc->destroy(_n);
    }

    inline void findHelper(Size _bucketIdx,
                           const KeyType & _key,
                           Node *& _outNode,
                           Node *& _prev) const
    {
        Bucket & b = m_buckets[_bucketIdx];
        _outNode = nullptr;
        _prev = nullptr;
        Node * n = b.first;

        while (n)
        {
            if (_key == n->kv.key)
            {
                _outNode = n;
                STICK_ASSERT(n->prev == _prev);
                return;
            }
            _prev = n;
            n = n->next;
        }
    }

    // inline void findByHandleHelper(const Handle & _handle, Node *& _outNode, Node *& _prev) const
    // {
    //     Bucket & b = m_buckets[_handle.bucketIndex];
    //     _outNode = nullptr;
    //     _prev = nullptr;
    //     Node * n = b.first;

    //     while (n)
    //     {
    //         if (_handle.id == n->id)
    //         {
    //             _outNode = n;
    //             STICK_ASSERT(n->prev == _prev);
    //             return;
    //         }
    //         _prev = n;
    //         n = n->next;
    //     }
    // }

    // template<class IterT, class MapType>
    // inline static IterT findByHandle(MapType && _map, const Handle & _handle)
    // {
    //     if (_handle.bucketIndex < _map.m_bucketCount)
    //     {
    //         Node * n, * prev;
    //         _map.findByHandleHelper(_handle, n , prev);
    //         if (!n)
    //             return _map.end();
    //         else
    //             return IterT(std::forward<MapType>(_map), _handle.bucketIndex, n);
    //     }

    //     return _map.end();
    // }

    template <class IterT, class MapType>
    inline static IterT findByKey(MapType && _map, const KeyType & _key)
    {
        Size bi = _map.bucketIndex(_key);
        Node *n, *prev;
        _map.findHelper(bi, _key, n, prev);
        if (!n)
            return _map.end();
        else
            return IterT(std::forward<MapType>(_map), bi, n);
    }

    inline Bucket * allocateBuckets(Size _i, Size * _outSize)
    {
        auto mem = m_alloc->allocate(sizeof(Bucket) * _i, alignof(Bucket));
        *_outSize = mem.size;
        return new (mem.ptr) Bucket[_i];
    }

    inline Node * copyNode(Node * _node)
    {
        STICK_ASSERT(_node);
        Node * ret = createNode({ _node->kv.key, _node->kv.value }, _node->bucketIndex);
        if (_node->next)
        {
            Node * next = copyNode(_node->next);
            next->prev = ret;
            ret->next = next;
        }
        return ret;
    }

    Allocator * m_alloc;
    Bucket * m_buckets;
    Size m_bucketAllocationSize;
    Size m_bucketCount;
    Float32 m_maxLoadFactor;
    Hash m_hasher;
    Size m_count;
    Size m_nextNodeID;
};
} // namespace stick

#endif // STICK_HASHMAP_HPP
