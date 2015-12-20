#ifndef STICK_STICKRBTREE_HPP
#define STICK_STICKRBTREE_HPP

#include <Stick/StickAllocator.hpp>
#include <Stick/StickIterator.hpp>
#include <Stick/StickUtility.hpp>
#include <new> //for placement new

#include <iostream>

namespace stick
{
    template<class T>
    class RBTree 
    {
    public:

        typedef T ValueType;

        enum class Color : bool
        {
            Red = true,
            Black = false
        };

        struct Node
        {
            Node() :
            color(Color::Red),
            left(nullptr),
            right(nullptr),
            parent(nullptr)
            {

            }

            Node(const T & _value) :
            color(Color::Red),
            value(_value),
            left(nullptr),
            right(nullptr),
            parent(nullptr)
            {

            }

            inline Node * grandparent()
            {
                if (parent != nullptr)
                    return parent->parent;
                return nullptr;
            }

            inline Node * uncle()
            {
                Node * g = grandparent();
                if (g == nullptr)
                    return nullptr;
                if (parent == g->left)
                    return g->right;
                else
                    return g->left;
            }

            inline Node * sibling()
            {   
                if(!parent) return nullptr;

                if (this == parent->left)
                    return parent->right;
                else
                    return parent->left;
            }

            inline void swapValue(Node & _other)
            {
                T tmp = _other.value;
                _other.value = value;
                value = tmp;
            }

            Color color;
            T value;
            Node * left;
            Node * right;
            Node * parent;
        };

        struct InsertResult
        {
            Node * node;
            bool inserted;
        };

        RBTree(Allocator & _alloc = defaultAllocator()) :
        m_alloc(&_alloc),
        m_elementCount(0)
        {
        }

        ~RBTree()
        {   
            clear();
        }

        template<class K>
        inline Node * find(const K & _val)
        {
            return m_rootNode ? findImpl(m_rootNode, _val) : nullptr;
        }

        inline Node * root() const
        {
            return m_rootNode;
        }

        inline InsertResult insert(const ValueType & _val)
        {
            if(m_elementCount == 0)
            {
                m_rootNode = createNode(_val);
                m_rootNode->color = Color::Black;
                m_elementCount++;
                return {m_rootNode, true};
            }
            else
            {
                return insertImpl(m_rootNode, _val);
            }
        }

        inline bool remove(const ValueType & _val)
        {
            Node * n = find(_val);
            if(n)
            {
                removeNode(n);
                return true;
            }
            return false;
        }

        inline void removeNode(Node * _n)
        {
            removeImpl(_n);
            m_elementCount--;
        }

        inline void clear()
        {
            if(m_rootNode)
                deallocateTree(m_rootNode);
        }

        inline Size elementCount() const
        {
            return m_elementCount;
        }

        inline Node * rightMost() const
        {
            if(m_rootNode)
                return recurseRight(m_rootNode);

            return nullptr;
        }

    private:

        inline Node * recurseRight(Node * _node) const
        {
            if(_node->right)
                return recurseRight(_node->right);

            return _node;
        }

        inline void deallocateTree(Node * _node)
        {
            if(_node->left)
                deallocateTree(_node->left);
            if(_node->right)
                deallocateTree(_node->right);

            destroyNode(_node);
        }

        inline Node * createNode(const ValueType & _val)
        {
            auto mem = m_alloc->allocate(sizeof(Node));
            STICK_ASSERT(mem.ptr);
            return new (mem.ptr) Node(_val);
        }

        inline void destroyNode(Node * _n)
        {
            _n->~Node();
            m_alloc->deallocate({_n, sizeof(Node)});
        }

        template<class K>
        inline Node * findImpl(Node * _currentNode, const K & _val)
        {
            if(_currentNode->value == _val)
            {
                return _currentNode;
            }
            else if(_currentNode->value > _val)
            {
                if(!_currentNode->left)
                {
                    return nullptr;
                }
                else
                {
                    return findImpl(_currentNode->left, _val);
                }
            }
            else
            {
                if(!_currentNode->right)
                {
                    return nullptr;
                }
                else
                {
                    return findImpl(_currentNode->right, _val);
                }
            }
        }

        inline void removeImpl(Node * _node)
        {
            STICK_ASSERT(m_elementCount >= 0);

            UInt8 nullChildrenCount = 0;
            if(!_node->left) nullChildrenCount++;
            if(!_node->right) nullChildrenCount++;

            if(nullChildrenCount == 0)
            {
                Node* p = _node->left;
                while (p->left) p = p->right;
                p->swapValue(*_node);
                removeImpl(p);
            }
            else if(nullChildrenCount == 1)
            {
                Node * child = _node->left ? _node->left : _node->right;
                STICK_ASSERT(_node->color == Color::Black && child->color == Color::Red);
                child->color = Color::Black;
                if (_node->parent)
                {
                    child->parent = _node->parent;
                    if (_node->parent->left == _node)
                        _node->parent->left = child;
                    else 
                        _node->parent->right = child;
                } 
                else 
                {
                    m_rootNode = child;
                    child->parent = nullptr;
                }
            }
            else if(nullChildrenCount == 2)
            {
                if(_node == m_rootNode)
                {
                    destroyNode(_node);
                    m_rootNode = nullptr;
                }
                else
                {
                    if (_node->parent->left == _node)
                        _node->parent->left = nullptr;
                    else 
                        _node->parent->right = nullptr;
                    Node * p = _node->parent;
                    auto col = _node->color;
                    destroyNode(_node);
                    //TODO: if black, fix tree
                    if(col == Color::Black)
                        deleteFix(p);
                }
            }
        }

        inline void deleteFix(Node * _node)
        {
            if(!_node->parent || !_node->parent->parent)
                return;

            Node * sibling;
            bool bLeft = false;
            if(_node == _node->parent->left)
                sibling = _node->parent->right;
            else
            {
                bLeft = true;
                sibling = _node->parent->left;
            }

            if(sibling->color == Color::Red)
            {
                sibling->color = sibling->parent->color;
                sibling->parent->color = sibling->parent->color == Color::Red ? Color::Black : Color::Red;
                rotate(sibling, !bLeft);
                deleteFix(_node);
            }
            else if(_node->parent->color == Color::Black && sibling->color == Color::Black &&
                    (!sibling->left || sibling->left->color == Color::Black) &&
                    (!sibling->right || sibling->right->color == Color::Black))
            {
                sibling->color = Color::Red;
                deleteFix(_node->parent);
            }
            else if(_node->parent->color == Color::Red)
            {
                STICK_ASSERT((!sibling->left || sibling->left->color == Color::Red) && (!sibling->right || sibling->right->color == Color::Red));
                sibling->color = sibling->parent->color;
                sibling->parent->color = sibling->parent->color == Color::Red ? Color::Black : Color::Red;
            }
            else if((!bLeft && (sibling->right || sibling->right->color == Color::Black) && sibling->left->color == Color::Red) ||
                    (bLeft && (sibling->left || sibling->left->color == Color::Black) && sibling->right->color == Color::Red))
            {
                Node * child = bLeft ? sibling->right : sibling->left;
                sibling->color = child->color;
                child->color = child->color == Color::Red ? Color::Black : Color::Red;
                rotate(child, bLeft);
                deleteFix(_node);
            }
            else if((!bLeft && sibling->right && sibling->right->color == Color::Red) ||
                    (bLeft && sibling->left && sibling->left->color == Color::Red))
            {
                auto tmpCol = sibling->color;
                sibling->color = sibling->parent->color;
                sibling->parent->color = tmpCol;
                if(bLeft)
                    sibling->right->color = Color::Black;
                else
                    sibling->left->color = Color::Black;
                rotate(sibling, !bLeft);
            }
        }

        inline InsertResult insertImpl(Node * _currentNode, const ValueType & _val)
        {
            if(_currentNode->value == _val)
            {
                //assign the value, as the comparision does not necessarily mean they are identical
                _currentNode->value = _val;
                return {_currentNode, false};
            }
            else
            {
                Node * node = nullptr;
                bool bLeft = false;
                if(_val < _currentNode->value)
                {
                    bLeft = true;
                    node = _currentNode->left;
                }
                else
                    node = _currentNode->right;

                if(!node)
                {
                    node = createNode(_val);
                    node->parent = _currentNode;
                    if(bLeft)
                        _currentNode->left = node;
                    else
                        _currentNode->right = node;
                    m_elementCount++;

                    //fix the balancing of the tree after creating the new node
                    insertFix(node);
                    return {node, true};
                }
                else
                {
                    return insertImpl(node, _val);
                }
            }
        }

        inline void insertFix(Node * _node)
        {   
            if (_node->parent == nullptr && _node->color == Color::Red) 
            {   // Root is wrong color
                STICK_ASSERT(_node == m_rootNode);
                _node->color = Color::Black;
                return;
            } 

            //case 2: tree stil valid
            if(_node->parent->color == Color::Black)
            {
                return;
            }
            if (_node->color == Color::Red && _node->parent->color == Color::Red)
            {
                //case 3: parent and uncle are red
                Node * uncle = _node->uncle();
                if((uncle != nullptr) && (uncle->color == Color::Red))
                {
                    _node->parent->color = Color::Black;
                    uncle->color = Color::Black;
                    Node * g = _node->grandparent();
                    g->color = Color::Red;
                    //fix what might be broken after these changes
                    insertFix(g);
                }
                else
                {
                    //case 4: Parent is red, uncle is black -> rotate to fix
                    Node * g = _node->grandparent();
                    if((_node == _node->parent->right) && (_node->parent == g->left))
                    {
                        rotate(_node, true);
                        insertFix(_node->left);
                    }
                    else if((_node == _node->parent->left) && (_node->parent == g->right))
                    {
                        rotate(_node, false);
                        insertFix(_node->right);
                    }
                    else
                    {
                        //case 5: Parent and Child are on same side.
                        Node * g = _node->grandparent();
                        _node->parent->color = g->color;
                        g->color = g->color == Color::Red ? Color::Black : Color::Red;
                        if(_node == _node->parent->left)
                            rotate(_node->parent, false);
                        else
                            rotate(_node->parent, true);
                    }
                }
            }
        }

        inline void rotate(Node * _node, bool _bLeft)
        {
            Node* p = _node->parent;
            Node* g = p->parent;
            _node->parent = g;
            p->parent = _node;
            Node* gc;
            if (_bLeft) 
            { 
                gc = _node->left;
                _node->left = p;
                p->right = gc;
            } 
            else 
            {
                gc = _node->right;
                _node->right = p;
                p->left = gc;
            }
            if (gc) 
                gc->parent = p;

            if (!g) 
                m_rootNode = _node;
            else if (g->left == p) 
                g->left = _node;
            else 
                g->right = _node;
        }

        Allocator * m_alloc;
        Node * m_rootNode;
        Size m_elementCount;
    };
}

#endif //STICK_STICKRBTREE_HPP
