#ifndef STICK_MAP_HPP
#define STICK_MAP_HPP

#include <Stick/StickRBTree.hpp>
#include <Stick/StickIterator.hpp>

namespace stick
{
    template<class K, class V>
    class Map
    {
    public:

        typedef K KeyType;
        typedef V ValueType;

        struct KeyValuePair
        {
            KeyValuePair(const KeyType & _key, const ValueType & _value) :
                key(_key),
                value(_value)
            {

            }

            //we only compare keys
            bool operator == (const KeyValuePair & _other) const
            {
                return key == _other.key;
            }

            bool operator != (const KeyValuePair & _other) const
            {
                return key != _other.key;
            }

            bool operator == (const KeyType & _other) const
            {
                return key == _other;
            }

            bool operator != (const KeyType & _other) const
            {
                return key != _other;
            }

            bool operator < (const KeyValuePair & _other) const
            {
                return key < _other.key;
            }

            bool operator > (const KeyValuePair & _other) const
            {
                return key > _other.key;
            }

            bool operator < (const KeyType & _other) const
            {
                return key < _other;
            }

            bool operator > (const KeyType & _other) const
            {
                return key > _other;
            }

            KeyType key;
            ValueType value;
        };

        typedef RBTree<KeyValuePair> Representation;

        typedef typename Representation::Node Node;

        template<class T>
        struct IterT
        {
            typedef typename T::Node NodeType;

            typedef typename T::KeyValuePair ValueType;

            typedef ValueType & ReferenceType;

            typedef ValueType * PointerType;


            IterT() :
                current(nullptr),
                last(nullptr)
            {

            }

            IterT(NodeType * _node, NodeType * _last) :
                current(_node),
                last(_last)
            {

            }

            inline void increment()
            {
                //TODO: I am pretty sure this function can be more compact and simple
                if (!current)
                    return;

                if (current->left)
                {
                    current = current->left;
                }
                else if (current->right)
                {
                    current = current->right;
                }
                else
                {
                    if (current == last)
                    {
                        current = nullptr;
                        return;
                    }

                    while (current->parent && (!current->parent->right || current->parent->right == current))
                    {
                        current = current->parent;
                    }
                    if (current->parent)
                        current = current->parent->right;
                    else
                        current = last;
                }
            }

            inline void decrement()
            {
                if (!current)
                    return;

                if (!current->parent)
                {
                    current = nullptr;
                    return;
                }

                if (current == current->parent->left)
                    current = current->parent;
                else
                {
                    if (!current->parent->left)
                        current = current->parent;
                    else
                    {
                        current = current->parent->left;
                        while (!current->right)
                        {
                            if (current->left)
                                current = current->left;
                            else
                                break;
                        }
                        if (current->right)
                        {
                            while (current->right)
                                current = current->right;
                        }
                    }
                }
            }

            inline bool operator == (const IterT & _other) const
            {
                return current == _other.current;
            }

            inline bool operator != (const IterT & _other) const
            {
                return current != _other.current;
            }

            inline IterT & operator--()
            {
                decrement();
                return *this;
            }

            inline IterT operator--(int)
            {
                Iter ret = *this;
                decrement();
                return ret;
            }

            inline IterT & operator-=(Size _i)
            {
                for (Size i = 0; i <= _i; ++i)
                    decrement();
                return *this;
            }

            inline IterT operator-(Size _i) const
            {
                Iter ret = *this;
                for (Size i = 0; i <= _i; ++i)
                    ret.decrement();
                return ret;
            }

            inline IterT & operator++()
            {
                increment();
                return *this;
            }

            inline IterT operator++(int)
            {
                Iter ret = *this;
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
                Iter ret = *this;
                for (Size i = 0; i <= _i; ++i)
                    ret.increment();
                return ret;
            }

            inline KeyValuePair & operator * () const
            {
                return current->value;
            }

            inline KeyValuePair  * operator -> () const
            {
                return &current->value;
            }

            NodeType * current;
            NodeType * last;
        };

        typedef IterT<Map> Iter;

        typedef IterT<const Map> ConstIter;

        typedef ReverseIterator<Iter> ReverseIter;

        typedef ReverseIterator<ConstIter> ReverseConstIter;


        struct InsertResult
        {
            Iter iterator;
            bool inserted;
        };

        inline InsertResult insert(const KeyValuePair & _val)
        {
            auto res = m_tree.insert(_val);
            return {Iter(res.node, m_tree.rightMost()), res.inserted};
        }

        inline InsertResult insert(const KeyType & _key, const ValueType & _val)
        {
            return insert({_key, _val});
        }

        ValueType & operator [] (const KeyType & _key)
        {
            Node * n = m_tree.find(_key);
            if (n)
            {
                return n->value.value;
            }
            else
            {
                auto res = m_tree.insert({_key, ValueType()});
                return res.node->value.value;
            }
        }

        inline Iter find(const KeyType & _key)
        {
            return Iter(m_tree.find(_key), m_tree.rightMost());
        }

        inline ConstIter find(const KeyType & _key) const
        {
            return ConstIter(m_tree.find(_key), m_tree.rightMost());
        }

        inline Iter remove(Iter _it)
        {
            Iter ret = _it + 1;
            m_tree.removeNode(_it.current);
            return ret;
        }

        inline Iter remove(const KeyType & _key)
        {
            Iter it = find(_key);
            if (it != end())
            {
                m_tree.removeNode(it.current);
                return ++it;
            }
            return Iter();
        }

        inline Iter begin()
        {
            return Iter(m_tree.root(), m_tree.rightMost());
        }

        inline ConstIter begin() const
        {
            return ConstIter(m_tree.root(), m_tree.rightMost());
        }

        inline ReverseIter rbegin()
        {
            Node * rm = m_tree.rightMost();
            return ReverseIter(Iter(rm, rm));
        }

        inline ReverseConstIter rbegin() const
        {
            Node * rm = m_tree.rightMost();
            return ReverseConstIter(ConstIter(rm, rm));
        }

        inline Iter end()
        {
            return Iter();
        }

        inline ConstIter end() const
        {
            return ConstIter();
        }

        inline ReverseIter rend()
        {
            return ReverseIter(Iter());
        }

        inline ReverseConstIter rend() const
        {
            return ReverseConstIter(ConstIter());
        }

        inline Size elementCount() const
        {
            return m_tree.elementCount();
        }

        inline void clear() const
        {
            m_tree.clear();
        }

    private:

        Representation m_tree;
    };
}

#endif //STICK_MAP_HPP
