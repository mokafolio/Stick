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
                return _key == _other.key;
            }

            bool operator != (const KeyValuePair & _other) const
            {
                return _key != _other.key;
            }

            bool operator < (const KeyValuePair & _other) const
            {
                return _key < _other.key;
            }

            bool operator > (const KeyValuePair & _other) const
            {
                return _key > _other.key;
            }

            KeyType key;
            ValueType value;
        };

        typedef RBTree<KeyValuePair> Representation;

        typedef typename Representation::Node Node;

        struct Iter
        {
            Iter() :
            bLeft(true),
            current(nullptr),
            parent(nullptr)
            {

            }

            Iter(Node * _node) :
            bLeft(true),
            current(_node),
            parent(_node->parent)
            {

            }

            void increment()
            {
                if(current->left && bLeft)
                {
                    parent = current;
                    current = current->left;
                    bLeft = true;
                }
                else if(current->right)
                {
                    parent = current;
                    current = current->right;
                    bLeft = false;
                }
                else
                {
                    
                }
            }

            void decrement()
            {

            }

            bool bLeft;
            Node * current;
            Node * parent;
        };
    };
}

#endif //STICK_MAP_HPP
