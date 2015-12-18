#include <Stick/StickString.hpp>
#include <Stick/StickDynamicArray.hpp>
#include <Stick/StickRBTree.hpp>
#include <Stick/StickMap.hpp>

#include <iostream>

using namespace stick;

/*
void printNode(const char * _prefix, RBTree<Int32> & _tree, RBTree<Int32>::Node * _n, Size _depth)
{
    const char * col = _n->color == RBTree<Int32>::Color::Red ? "red" : "black";
    std::cout<<_prefix<<" depth: "<<_depth++<<" VAL: "<<_n->value<<" Col: "<<col<<std::endl;

    if(_n->left)
    {
        printNode("LFT", _tree, _n->left, _depth);
    }
    if(_n->right)
    {
        printNode("RGT: ", _tree, _n->right, _depth);
    }
}*/


int main(int _argc, const char * _args[])
{
    SUITE("String Tests")
    {   
        String a("test");
        String b("test");
        String c(String("different"));
        TEST(a == b);
        TEST(a != c);
        TEST(b != c);

        char expectedResults[] = {'t', 'e', 's', 't'};

        Int32 i = 0;
        for(auto c : a)
        {
            TEST(c == expectedResults[i]);
            i++;
        }

        i = 3;
        for(auto it = a.rbegin(); it != a.rend(); it++)
        {
            TEST(*it == expectedResults[i]);
            i--;
        }

        String d;
        d = a;
        TEST(d == a);
        TEST(d == "test");

        d = "another one";
        TEST(d == "another one");
    }

    SUITE("DynamicArray Tests")
    {
        DynamicArray<Int32> a;
        TEST(a.elementCount() == 0);
        TEST(a.byteCount() == 0);

        a.resize(5);
        TEST(a.elementCount() == 5);
        TEST(a.byteCount() == 20);

        for(Int32 i = 0; i < 5; ++i)
        {
            a[i] = i;
        }

        TEST(a[0] == 0);
        TEST(a[1] == 1);
        TEST(a[2] == 2);
        TEST(a[3] == 3);
        TEST(a[4] == 4);

        a.append(10);
        TEST(a.elementCount() == 6);

        TEST(a[5] == 10);
        TEST(a.back() == 10);
        TEST(a.front() == 0);

        a.popBack();
        TEST(a.back() == 4);
        TEST(a.elementCount() == 5);

        Int32 expectedResults[] = {0, 1, 2, 3, 4};
        Int32 i = 0;
        for(auto e : a)
        {
            TEST(e == expectedResults[i]);
            ++i;
        }

        i = a.elementCount() - 1;
        for(auto it = a.rbegin(); it != a.rend(); ++it)
        {
            TEST(*it == expectedResults[i]);
            --i;
        }

        a.clear();
        TEST(a.elementCount() == 0);
        
        DynamicArray<Int32> b;
        Int32 arr[] = {1, 2, 3, 4};
        b.append(10);
        b.append(11);
        b.append(12);
        b.append(13);
        auto it = b.insert(b.end(), arr, arr + 4);
        TEST(it == b.end() - 4);
        TEST(it == b.begin() + 4);
        TEST(*it == 1);
        auto it2 = b.insert(b.begin() + 2, 99);
        TEST(it2 == b.begin() + 2);
        TEST(*it2 == 99);

        Int32 expectedArr[] = {10, 11, 99, 12, 13, 1, 2, 3, 4};
        i = 0;
        for(auto v : b)
        {
            TEST(expectedArr[i] == v);
            i++;
        }
        TEST(b.elementCount() == 9);

        auto it3 = b.erase(b.begin() + 2, b.begin() + 6);
        TEST(it3 == b.begin() + 2);
        TEST(*it3 == 2);
        Int32 expectedArr2[] = {10, 11, 2, 3, 4};
        i=0;
        for(auto v : b)
        {
            TEST(expectedArr2[i] == v);
            i++;
        }
    }

    SUITE("RBTree Tests")
    {
        RBTree<Int32> tree;
        TEST(tree.elementCount() == 0);

        tree.insert(6);

        TEST(tree.root()->value == 6);

        tree.insert(5);
        tree.insert(7);
        tree.insert(9);
        tree.insert(8);
        tree.insert(1);
        tree.insert(24);
        tree.insert(2);
        tree.insert(3);

        TEST(tree.elementCount() == 9);

        auto n = tree.find(5);
        auto n2 = tree.find(2);
        auto n3 = tree.find(24);

        TEST(n != nullptr);
        TEST(n2 != nullptr);
        TEST(n3 != nullptr);

        TEST(n->value == 5);
        TEST(n2->value == 2);
        TEST(n3->value == 24);

        auto notFound = tree.find(199);
        TEST(notFound == nullptr);

        tree.remove(1);
        tree.remove(8);
        tree.remove(24);
        TEST(tree.elementCount() == 6);

        auto n4 = tree.find(1);
        auto n5 = tree.find(8);
        auto n6 = tree.find(24);

        TEST(n4 == nullptr);
        TEST(n5 == nullptr);
        TEST(n6 == nullptr);
    }

    SUITE("Map Tests")
    {
        Map<String, Int32> map;
        auto res = map.insert("a", 1);

        TEST(res.inserted == true);
        TEST((*res.iterator).value == 1);
        TEST(res.iterator->value == 1);
        TEST(res.iterator->key == "a");
        TEST(map.elementCount() == 1);

        auto res2 = map.insert("a", 2);
        TEST(res2.inserted == false);
        TEST(res2.iterator->value == 2);
        TEST(map.elementCount() == 1);
        
        map.insert("b", 3);
        map.insert("c", 4);
        TEST(map.elementCount() == 3);

        auto it = map.find("b");
        TEST(it != map.end());
        TEST(it->value == 3);

        it = map.find("notThere");
        TEST(it == map.end());

        map.insert("d", 5);
        map.insert("e", 6);

        /*it = map.begin();
        std::cout<<it->key.cString()<<": "<<it->value<<std::endl;
        it++;
        std::cout<<it->key.cString()<<": "<<it->value<<std::endl;
        it++;
        std::cout<<it->key.cString()<<": "<<it->value<<std::endl;
        it++;
        std::cout<<it->key.cString()<<": "<<it->value<<std::endl;
        it++;
        std::cout<<it->key.cString()<<": "<<it->value<<std::endl;*/

        for(const auto & kv : map)
        {
            std::cout<<kv.key.cString()<<": "<<kv.value<<std::endl;
        }
    }
}
