#ifndef STICK_MAP_HPP
#define STICK_MAP_HPP

#include <Stick/StickRBTree.hpp>

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

            bool operator < (const KeyValuePair & _other) const
            {
                return key < _other.key;
            }

            bool operator > (const KeyValuePair & _other) const
            {
                return key > _other.key;
            }

            KeyType key;
            ValueType value;
        };

        typedef RBTree<KeyValuePair> Representation;

        typedef typename Representation::Node Node;

        struct Iter
        {
            Iter() :
            current(nullptr)
            {

            }

            Iter(Node * _node) :
            current(_node)
            {

            }

            inline void increment()
            {
                if(current->left)
                {
                    current = current->left;
                }
                else if(current->right)
                {
                    current = current->right;
                }
                else
                {
                    while(current->parent && !current->parent->right)
                    {
                        current = current->parent;
                    }
                    if(current) current = current->right;
                }
            }

            inline void decrement()
            {

            }

            inline bool operator == (const Iter & _other) const
            {
                return current == _other.current;
            }

            inline bool operator != (const Iter & _other) const
            {
                return current != _other.current;
            }

            inline Iter & operator--() 
            {
                decrement();
                return *this;
            } 

            inline Iter operator--(int) 
            {
                Iter ret = *this;
                decrement();
                return ret;
            }

            inline Iter & operator-=(Size _i) 
            {
                for(Size i=0; i<=_i; ++i)
                    decrement();
                return *this;
            }

            inline Iter operator-(Size _i) const
            {
                Iter ret = *this;
                for(Size i=0; i<=_i; ++i)
                    ret.decrement();
                return ret;
            }

            inline Iter & operator++() 
            {
                increment();
                return *this;
            } 

            inline Iter operator++(int) 
            {
                Iter ret = *this;
                increment();
                return ret;
            }

            inline Iter & operator+=(Size _i) 
            {
                for(Size i=0; i<=_i; ++i)
                    increment();
                return *this;
            }

            inline Iter operator+(Size _i) const
            {
                Iter ret = *this;
                for(Size i=0; i<=_i; ++i)
                    ret.increment();
                return ret;
            }

            inline KeyValuePair & operator * () const
            {
                return current->value;
            }

            inline KeyValuePair *  operator -> () const
            {
                return &current->value;
            }

            Node * current;
        };

        typedef const Iter ConstIter;

        struct InsertResult
        {
            Iter iterator;
            bool inserted;
        };

        inline InsertResult insert(const KeyValuePair & _val)
        {
            auto res = m_tree.insert(_val);
            return {Iter(res.node), res.inserted};
        }

        inline InsertResult insert(const KeyType & _key, const ValueType & _val)
        {
            return insert({_key, _val});
        }

        inline Iter find(const KeyType & _key)
        {
            return Iter(m_tree.find({_key, ValueType()}));
        }

        inline ConstIter find(const KeyType & _key) const
        {
            return ConstIter(m_tree.find({_key, ValueType()}));
        }

        inline Iter begin()
        {
            return Iter(m_tree.root());
        }

        inline ConstIter begin() const
        {
            return ConstIter(m_tree.root());
        }

        inline Iter end()
        {
            return Iter();
        }

        inline ConstIter end() const
        {
            return ConstIter();
        }

        inline Size elementCount() const
        {
            return m_tree.elementCount();
        }

    private:

        Representation m_tree;
    };
}

#endif //STICK_MAP_HPP
