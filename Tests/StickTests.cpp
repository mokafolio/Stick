#include <Stick/StickString.hpp>
#include <Stick/StickDynamicArray.hpp>
#include <Stick/StickRBTree.hpp>

#include <iostream>

using namespace stick;

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
        tree.insert(1);
        tree.insert(2);

        auto n = tree.find(1);
        auto n2 = tree.find(2);

        TEST(n->bIsRed == false);
        TEST(n2->bIsRed == true);
        TEST(n->value == 1);
        TEST(n2->value == 2);
        TEST(n != n2);
        TEST(tree.elementCount() == 2);

        auto oldn2 = n2;
        tree.insert(2);
        TEST(tree.elementCount() == 2);
        TEST(oldn2 == tree.find(2));
    }
}
