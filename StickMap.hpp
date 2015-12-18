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

            inline Iter operator-(Size _i) 
            {
                Iter ret = *this;
                for(Size i=0; i<=_i; ++i)
                    ret.decrement();
                return ret;
            }
            /*
            inline Iter & operator++() 
            {
                m_it--;
                return *this;
            } 

            inline Iter & operator++(int) 
            {
                m_it--;
                return *this;
            }

            inline Iter & operator+=(Size _i) 
            {
                m_it -= _i;
                return *this;
            }

            inline Node & operator * () const
            {
                return *current;
            }

            inline Node *  operator -> () const
            {
                return current;
            }*/

            Node * current;
        };
    };
}

#endif //STICK_MAP_HPP
