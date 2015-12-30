#include <Stick/StickString.hpp>
#include <Stick/StickDynamicArray.hpp>
#include <Stick/StickRBTree.hpp>
#include <Stick/StickMap.hpp>
#include <Stick/StickHashMap.hpp>
#include <Stick/StickError.hpp>
#include <Stick/StickThread.hpp>
#include <Stick/StickConditionVariable.hpp>
#include <Stick/StickHighResolutionClock.hpp>
#include <Stick/StickTest.hpp>
#include <limits>

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

class SomeClass
{
public:

    SomeClass()
    {
        std::cout << "SomeClass()" << std::endl;
    }

    ~SomeClass()
    {
        std::cout << "~SomeClass()" << std::endl;
    }
};

struct DestructorTester
{
    static int destructionCount;

    ~DestructorTester()
    {
        ++destructionCount;
    }

    static void reset()
    {
        destructionCount = 0;
    }
};

int DestructorTester::destructionCount = 0;


const Suite spec[] =
{
    SUITE("String Tests")
    {
        String a("test");
        String b("test");
        String c(String("different"));
        EXPECT(a == b);
        EXPECT(a != c);
        EXPECT(b != c);

        char expectedResults[] = {'t', 'e', 's', 't'};

        Int32 i = 0;
        for (auto c : a)
        {
            EXPECT(c == expectedResults[i]);
            i++;
        }

        i = 3;
        for (auto it = a.rbegin(); it != a.rend(); it++)
        {
            EXPECT(*it == expectedResults[i]);
            i--;
        }

        String d;
        d = a;
        EXPECT(d == a);
        EXPECT(d == "test");

        d = "another one";
        EXPECT(d == "another one");

        String e;
        EXPECT(e != d);
    },
    SUITE("DynamicArray Tests")
    {
        DynamicArray<Int32> a;
        EXPECT(a.elementCount() == 0);
        EXPECT(a.byteCount() == 0);

        a.resize(5);
        EXPECT(a.elementCount() == 5);
        EXPECT(a.byteCount() == 20);

        for (Int32 i = 0; i < 5; ++i)
        {
            a[i] = i;
        }

        EXPECT(a[0] == 0);
        EXPECT(a[1] == 1);
        EXPECT(a[2] == 2);
        EXPECT(a[3] == 3);
        EXPECT(a[4] == 4);

        a.append(10);
        EXPECT(a.elementCount() == 6);

        EXPECT(a[5] == 10);
        EXPECT(a.back() == 10);
        EXPECT(a.front() == 0);

        a.removeBack();
        EXPECT(a.back() == 4);
        EXPECT(a.elementCount() == 5);

        Int32 expectedResults[] = {0, 1, 2, 3, 4};
        Int32 i = 0;
        for (auto e : a)
        {
            EXPECT(e == expectedResults[i]);
            ++i;
        }

        i = a.elementCount() - 1;
        for (auto it = a.rbegin(); it != a.rend(); ++it)
        {
            EXPECT(*it == expectedResults[i]);
            --i;
        }

        a.clear();
        EXPECT(a.elementCount() == 0);

        DynamicArray<Int32> b;
        Int32 arr[] = {1, 2, 3, 4};
        b.append(10);
        b.append(11);
        b.append(12);
        b.append(13);
        auto it = b.insert(b.end(), arr, arr + 4);
        EXPECT(it == b.end() - 4);
        EXPECT(it == b.begin() + 4);
        EXPECT(*it == 1);
        auto it2 = b.insert(b.begin() + 2, 99);
        EXPECT(it2 == b.begin() + 2);
        EXPECT(*it2 == 99);

        Int32 expectedArr[] = {10, 11, 99, 12, 13, 1, 2, 3, 4};
        i = 0;
        for (auto v : b)
        {
            EXPECT(expectedArr[i] == v);
            i++;
        }
        EXPECT(b.elementCount() == 9);

        auto it3 = b.remove(b.begin() + 2, b.begin() + 6);
        EXPECT(it3 == b.begin() + 2);
        EXPECT(*it3 == 2);
        Int32 expectedArr2[] = {10, 11, 2, 3, 4};
        i = 0;
        for (auto v : b)
        {
            EXPECT(expectedArr2[i] == v);
            i++;
        }

        //destructor tests
        DynamicArray<DestructorTester> tt;
        tt.append(DestructorTester());
        tt.append(DestructorTester());
        tt.append(DestructorTester());
        tt.append(DestructorTester());
        tt.append(DestructorTester());
        DestructorTester::reset();
        tt.remove(tt.begin(), tt.begin() + 2);
        EXPECT(DestructorTester::destructionCount == 2);
        tt.clear();
        EXPECT(DestructorTester::destructionCount == 5);

        DynamicArray<Int32> ttt({1, 2, 3, 4, 5});
        EXPECT(ttt.elementCount() == 5);
        EXPECT(ttt[0] == 1);
        EXPECT(ttt[1] == 2);
        EXPECT(ttt[2] == 3);
        EXPECT(ttt[3] == 4);
        EXPECT(ttt[4] == 5);
    },
    SUITE("RBTree Tests")
    {
        RBTree<Int32> tree;
        EXPECT(tree.elementCount() == 0);

        tree.insert(6);

        EXPECT(tree.root()->value == 6);

        tree.insert(5);
        tree.insert(7);
        tree.insert(9);
        tree.insert(8);
        tree.insert(1);
        tree.insert(24);
        tree.insert(2);
        tree.insert(3);

        EXPECT(tree.elementCount() == 9);

        auto n = tree.find(5);
        auto n2 = tree.find(2);
        auto n3 = tree.find(24);

        EXPECT(n != nullptr);
        EXPECT(n2 != nullptr);
        EXPECT(n3 != nullptr);

        EXPECT(n->value == 5);
        EXPECT(n2->value == 2);
        EXPECT(n3->value == 24);

        auto notFound = tree.find(199);
        EXPECT(notFound == nullptr);

        tree.remove(1);
        tree.remove(8);
        tree.remove(24);
        EXPECT(tree.elementCount() == 6);

        auto n4 = tree.find(1);
        auto n5 = tree.find(8);
        auto n6 = tree.find(24);

        EXPECT(n4 == nullptr);
        EXPECT(n5 == nullptr);
        EXPECT(n6 == nullptr);
    },
    SUITE("Map Tests")
    {
        typedef Map<String, Int32> TestMapType;
        TestMapType map;
        auto res = map.insert("a", 1);

        EXPECT(res.inserted == true);
        EXPECT((*res.iterator).value == 1);
        EXPECT(res.iterator->value == 1);
        EXPECT(res.iterator->key == "a");
        EXPECT(map.elementCount() == 1);

        auto res2 = map.insert("a", 2);
        EXPECT(res2.inserted == false);
        EXPECT(res2.iterator->value == 2);
        EXPECT(map.elementCount() == 1);

        map.insert("b", 3);
        map.insert("c", 4);
        EXPECT(map.elementCount() == 3);

        auto it = map.find("b");
        EXPECT(it != map.end());
        EXPECT(it->value == 3);

        it = map.find("notThere");
        EXPECT(it == map.end());

        map.insert("d", 5);
        map.insert("e", 6);

        map["e"] = 7;
        map["f"] = 8;
        it = map.find("e");
        EXPECT(it->value == 7);
        EXPECT(map["f"] == 8);
        EXPECT(map.elementCount() == 6);

        it = map.begin();

        auto lastKey = it->key;
        auto lastVal = it->value;

        it++;
        EXPECT(it->key != lastKey);
        EXPECT(it->value != lastVal);
        --it;
        EXPECT(it->key == lastKey);
        EXPECT(it->value == lastVal);


        TestMapType::KeyType * expectedKeys = new TestMapType::KeyType[map.elementCount()];
        TestMapType::ValueType * expectedVals = new TestMapType::ValueType[map.elementCount()];

        Size i = 0;
        for (const auto & kv : map)
        {
            expectedKeys[i] = kv.key;
            expectedVals[i] = kv.value;
            i++;
        }

        EXPECT(i == map.elementCount());

        --i;
        Size j = 0;
        for (auto rit = map.rbegin(); rit != map.rend(); ++rit)
        {
            EXPECT(expectedKeys[i] == rit->key);
            EXPECT(expectedVals[i] == rit->value);
            ++j;
            --i;
        }

        EXPECT(j == map.elementCount());

        delete [] expectedVals;
        delete [] expectedKeys;

        //remove tests
        it = map.find("f");
        auto it3 = it + 1;
        auto it2 = map.remove(it);
        EXPECT(it3 == it2);
        EXPECT(map.elementCount() == 5);

        it = map.find("c");
        it++;
        it2 = map.remove("c");
        EXPECT(it == it2);
        EXPECT(map.elementCount() == 4);

        Map<Int32, String> amap = {{3, "test"}, {2, "bla"}, {199, "blubb"}};
        EXPECT(amap.elementCount() == 3);
        auto tit = amap.find(3);
        EXPECT(tit->key == 3);
        EXPECT(tit->value == "test");
        tit = amap.find(2);
        EXPECT(tit->key == 2);
        EXPECT(tit->value == "bla");
        tit = amap.find(199);
        EXPECT(tit->key == 199);
        EXPECT(tit->value == "blubb");
    },
    SUITE("HashMap Tests")
    {
        HashMap<String, Int32> hm(1);

        hm.insert("test", 1);
        hm.insert("test", 2);
        EXPECT(hm.elementCount() == 1);
        hm.insert("anotherKey", 3);
        auto res = hm.insert("blubb", 4);
        EXPECT(hm.elementCount() == 3);
        EXPECT(res.iterator->key == "blubb");
        EXPECT(res.iterator->value == 4);
        EXPECT(res.inserted == true);
        res = hm.insert("blubb", 5);
        EXPECT(res.iterator->value == 5);
        EXPECT(res.inserted == false);
        auto it = hm.remove("anotherKey");
        EXPECT(hm.elementCount() == 2);
        //TODO: More
    },
    SUITE("Thread Tests")
    {
        Thread thread;
        Error err = thread.join();
        EXPECT(err.code() != 0);
        std::atomic<bool> bJoinable(false);
        std::atomic<bool> bValidThreadID(false);
        err = thread.run([&]() { bValidThreadID = thread.threadID() != 0; bJoinable = thread.isJoinable(); });
        EXPECT(err.code() == 0);
        err = thread.join();
        EXPECT(bJoinable);
        EXPECT(bValidThreadID);
        EXPECT(err.code() == 0);
        EXPECT(!thread.isJoinable());
    },
    SUITE("ConditionVariable Tests")
    {
        Thread thread;
        ConditionVariable cond;
        Mutex m;
        Error err = thread.run([&]() { ScopedLock<Mutex> lock(m); cond.wait(lock); });
        auto start = HighResolutionClock::now();
        thread.sleepFor(Duration::fromSeconds(0.05));
        std::cout<<(HighResolutionClock::now() - start).seconds()<<std::endl;
        EXPECT(err.code() == 0);
        EXPECT(thread.isJoinable() == true); //the thread should still run as its blocking on the condition var
        err = cond.notifyOne();
        EXPECT(err.code() == 0);
        err = thread.join();
        EXPECT(err.code() == 0);
    }
};

int main(int _argc, const char * _args[])
{
    return runTests(spec, _argc, _args);
}
