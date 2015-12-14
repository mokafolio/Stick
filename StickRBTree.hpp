#ifndef STICK_STICKRBTREE_HPP
#define STICK_STICKRBTREE_HPP

#include <Stick/StickAllocator.hpp>
#include <Stick/StickIterator.hpp>
#include <Stick/StickUtility.hpp>
#include <new> //for placement new

namespace stick
{
    template<class T>
    class RBTree 
    {
    public:

        typedef T ValueType;

        struct Node
        {
            Node() :
            bIsRed(true),
            left(nullptr),
            right(nullptr),
            parent(nullptr)
            {

            }

            Node(const T & _value) :
            bIsRed(true),
            value(_value),
            left(nullptr),
            right(nullptr),
            parent(nullptr)
            {

            }

            bool bIsRed;
            T value;
            Node * left;
            Node * right;
            Node * parent;
        };

        RBTree(Allocator & _alloc = defaultAllocator()) :
        m_alloc(&_alloc),
        m_rootNode(nullptr),
        m_elementCount(0)
        {

        }

        ~RBTree()
        {
            if(m_rootNode)
                deallocateTree(m_rootNode);
        }

        Node * find(const ValueType & _val)
        {
            return m_rootNode ? findImpl(m_rootNode, _val) : nullptr;
        }

        void insert(const ValueType & _val)
        {
            if(!m_rootNode)
            {
                m_rootNode = createNode(_val);
                rebalance(m_rootNode);
                m_elementCount = 1;
            }
            else
            {
                insertImpl(m_rootNode, _val);
            }
        }

        Size elementCount() const
        {
            return m_elementCount;
        }

    private:

        void deallocateTree(Node * _node)
        {
            if(_node->left)
                deallocateTree(_node->left);
            if(_node->right)
                deallocateTree(_node->right);
            m_alloc->deallocate({_node, sizeof(Node)});
        }

        Node * createNode(const ValueType & _val)
        {
            auto mem = m_alloc->allocate(sizeof(Node));
            STICK_ASSERT(mem.ptr);
            return new (mem.ptr) Node(_val);
        }

        Node * findImpl(Node * _currentNode, const ValueType & _val)
        {
            if(_currentNode->value == _val)
            {
                return _currentNode;
            }
            else if(_currentNode->value > _val)
            {
                if(_currentNode->left == nullptr)
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
                if(_currentNode->right == nullptr)
                {
                    return nullptr;
                }
                else
                {
                    return findImpl(_currentNode->right, _val);
                }
            }
        }

        void insertImpl(Node * _currentNode, const ValueType & _val)
        {
            Node * insertedNode = nullptr;
            if(_currentNode->value == _val)
            {
                return;
            }
            else if(_currentNode->value > _val)
            {
                if(_currentNode->left == nullptr)
                {
                    insertedNode = createNode(_val);
                    _currentNode->left = insertedNode;
                    m_elementCount++;
                }
                else
                {
                    insertImpl(_currentNode->left, _val);
                }
            }
            else
            {
                if(_currentNode->right == nullptr)
                {
                    insertedNode = createNode(_val);
                    _currentNode->right = insertedNode;
                    m_elementCount++;
                }
                else
                {
                    insertImpl(_currentNode->right, _val);
                }
            }
            //restore uniform black height
            if (insertedNode == nullptr) return;
            rebalance(_currentNode);
        }

        inline static Node * grandparent(Node * _node)
        {
            if ((_node != nullptr) && (_node->parent != nullptr))
              return _node->parent->parent;
             else
              return nullptr;
        }

        inline static Node * uncle(Node * _node)
        {
            Node * g = grandparent(_node);
            if (g == nullptr)
                return nullptr; // No grandparent means no uncle
            if (_node->parent == g->left)
                return g->right;
            else
                return g->left;
        }

        inline static Node * sibling(Node * _node)
        {
            if (_node == _node->parent->left)
                return _node->parent->right;
            else
                return _node->parent->left;
        }

        inline static void rebalanceCase1(Node * _node)
        {
            if(_node->parent == nullptr)
                _node->bIsRed = false;
            else
                rebalanceCase2(_node);
        }

        inline static void rebalanceCase2(Node * _node)
        {
            if(!_node->parent->bIsRed)
            {
                rebalanceCase3(_node);
            }
        }

        inline static void rebalanceCase3(Node * _node)
        {
            Node * u = uncle(_node);
            if ((u != nullptr) && (u->bIsRed)) 
            {
                _node->parent->bIsRed = false;
                u->bIsRed = false;
                Node * g = grandparent(_node);
                g->bIsRed = false;
                rebalanceCase1(g);
            }
            else
            {
                rebalanceCase4(_node);
            }
        }

        inline static void rebalanceCase4(Node * _node)
        {
            Node * g = grandparent(_node);

            if ((_node == _node->parent->right) && (_node->parent == g->left)) 
            {
                rotateLeft(_node->parent);
                rebalanceCase5(_node->left);
            }
            else
            {
                rotateRight(_node->parent);
                rebalanceCase5(_node->right);
            }
        }

        inline static void rebalanceCase5(Node * _node)
        {
            Node * g = grandparent(_node);

            _node->parent->bIsRed = false;
            g->bIsRed = true;
            if (_node == _node->parent->left)
                rotateRight(g);
            else
                rotateLeft(g);
        }

        inline static void rotateLeft(Node * _node)
        {
            Node * nr = _node->right;

            if(_node->parent)
            {
                if(_node->parent->left == _node)
                    _node->parent->left = nr;
                else
                    _node->parent->right = nr;
            }

            nr->parent = _node->parent;
            _node->parent = nr;

            _node->right = nr->left;
            nr->left = _node;

            if(_node->right)
                _node->right->parent = _node;
        }

        inline static void rotateRight(Node * _node)
        {
            Node * nn = _node->left;

            if(_node->parent)
            {
                if(_node->parent->right == _node)
                    _node->parent->right = nn;
                else
                    _node->parent->left = nn;
            }

            nn->parent = _node->parent;
            _node->parent = nn;

            _node->left = nn->right;
            nn->right = _node;

            if(_node->left)
                _node->left->parent = _node;
        }

        static void rebalance(Node * _node)
        {
            rebalanceCase1(_node);
        }

        Allocator * m_alloc;
        Node * m_rootNode;
        Size m_elementCount;
    };
}

#endif //STICK_STICKRBTREE_HPP
