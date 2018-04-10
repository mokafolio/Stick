#include <Stick/ArgumentParser.hpp>
#include <Stick/String.hpp>
#include <Stick/DynamicArray.hpp>
#include <Stick/RBTree.hpp>
#include <Stick/Map.hpp>
#include <Stick/FixedArray.hpp>
#include <Stick/HashMap.hpp>
#include <Stick/Error.hpp>
#include <Stick/EventForwarder.hpp>
#include <Stick/Thread.hpp>
#include <Stick/ConditionVariable.hpp>
#include <Stick/HighResolutionClock.hpp>
#include <Stick/SystemClock.hpp>
#include <Stick/Test.hpp>
#include <Stick/StringConversion.hpp>
#include <Stick/Path.hpp>
#include <Stick/FileUtilities.hpp>
#include <Stick/TypeInfo.hpp>
#include <Stick/TypeList.hpp>
#include <Stick/UniquePtr.hpp>
#include <Stick/Maybe.hpp>
#include <Stick/FileSystem.hpp>
#include <Stick/Variant.hpp>
#include <Stick/StaticArray.hpp>

#include <Stick/Allocators/LinearAllocator.hpp>
#include <Stick/Allocators/GlobalAllocator.hpp>
#include <Stick/Allocators/Mallocator.hpp>
#include <Stick/Allocators/PoolAllocator.hpp>
#include <Stick/Allocators/FallbackAllocator.hpp>
#include <Stick/Allocators/FreeListAllocator.hpp>
#include <Stick/Allocators/Bucketizer.hpp>
#include <Stick/Allocators/Segregator.hpp>

#include <limits>
#include <atomic>

#include <thread>
#include <mutex>
#include <condition_variable>

using namespace stick;

struct TestEvent : public EventT<TestEvent>
{
    TestEvent(Int32 _num = 128) :
        someMember(_num)
    {

    }

    Int32 someMember = 128;
};

struct TestEvent2 : public EventT<TestEvent2>
{
    TestEvent2(Int32 _num = 64) :
        someMember(_num)
    {

    }

    Int32 someMember = 64;
};

static TestEvent lastTestEvent;
static bool bWasCalled = false;
static void freeFunctionCallback(const TestEvent & _evt)
{
    lastTestEvent = _evt;
    bWasCalled = true;
}

struct TestClass
{
    void memberCallback(const TestEvent & _evt)
    {
        counter++;
    }

    Int32 counter = 0;
};

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


struct ResultTestClass
{
    ResultTestClass()
    {
    }

    ResultTestClass(const ResultTestClass & _c)
    {
        printf("COPY ResultTestClass\n");
    }

    ResultTestClass(ResultTestClass && _c)
    {
        printf("MOVE ResultTestClass\n");
    }

    ResultTestClass & operator = (const ResultTestClass & _o)
    {
        printf("COP ASSIGN ResultTestClass\n");
        return *this;
    }

    ResultTestClass & operator = (ResultTestClass && _o)
    {
        printf("MOVE ASSIGN ResultTestClass\n");
        return *this;
    }
};

Result<ResultTestClass> makeResult()
{
    return ResultTestClass();
}


struct CustomAllocator : public Allocator
{
    using BucketizerPoolType = mem::PoolAllocator<mem::Mallocator,  mem::DynamicSizeFlag,  mem::DynamicSizeFlag, 256>;
    using SmallAllocator = mem::PoolAllocator<mem::Mallocator, 0, 8, 1024>;
    using MediumAllocator = mem::Bucketizer<BucketizerPoolType, 9, 512, 32>;

    using Allocator = mem::Segregator <
                      mem::T<8>, SmallAllocator,
                      mem::T<512>, MediumAllocator,
                      mem::Mallocator >;

    inline mem::Block allocate(Size _byteCount, Size _alignment) override
    {
        return m_alloc.allocate(_byteCount, _alignment);
    }

    inline void deallocate(const mem::Block & _block) override
    {
        m_alloc.deallocate(_block);
    }

    Allocator m_alloc;
};

struct NoMove
{
    NoMove()
    {

    }

    NoMove(const NoMove & _other)
    {

    }

    NoMove & operator = (const NoMove & _other)
    {
        return *this;
    }

    NoMove(NoMove && _other) = delete;
    NoMove & operator = (NoMove && _other) = delete;
};

//test classes for Resource tests
struct TestResource
{
    inline Error parse(const char * _data, Size _byteCount)
    {
        text = String(_data, _data + _byteCount);
        return Error();
    }

    String text;
};

const Suite spec[] =
{
    SUITE("String Tests")
    {
        String es;
        EXPECT(es.length() == 0);
        EXPECT(es == "");

        String a("test");
        String b("test");
        String c(String("different"));
        EXPECT(a == b);
        EXPECT(a != c);
        EXPECT(b != c);

        String emp;
        EXPECT(emp != a);
        EXPECT(b != emp);
        EXPECT(a != "");
        EXPECT(emp == "");

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

        String f;
        f.reserve(20);
        EXPECT(f.capacity() == 20);
        EXPECT(f.length() == 0);

        String ff("bla");
        ff.reserve(10);
        EXPECT(ff[0] == 'b');
        EXPECT(ff[1] == 'l');
        EXPECT(ff[2] == 'a');
        EXPECT(ff.length() == 3);

        String g;
        g.resize(10);
        EXPECT(g.length() == 10);

        String h("abcde");
        EXPECT(h[0] == 'a');
        EXPECT(h[1] == 'b');
        EXPECT(h[2] == 'c');
        EXPECT(h[3] == 'd');
        EXPECT(h[4] == 'e');
        h.resize(7, 'f');
        EXPECT(h.length() == 7);
        EXPECT(h[0] == 'a');
        EXPECT(h[1] == 'b');
        EXPECT(h[2] == 'c');
        EXPECT(h[3] == 'd');
        EXPECT(h[4] == 'e');
        EXPECT(h[5] == 'f');
        EXPECT(h[6] == 'f');

        {
            //test append function
            String i;
            String j(" World!");
            i.append("Hello");
            i.append(j);
            EXPECT(i.length() == 12);
            EXPECT(i[0] == 'H');
            EXPECT(i[1] == 'e');
            EXPECT(i[2] == 'l');
            EXPECT(i[3] == 'l');
            EXPECT(i[4] == 'o');
            EXPECT(i[5] == ' ');
            EXPECT(i[6] == 'W');
            EXPECT(i[7] == 'o');
            EXPECT(i[8] == 'r');
            EXPECT(i[9] == 'l');
            EXPECT(i[10] == 'd');
            EXPECT(i[11] == '!');

            i.append('c');
            EXPECT(i.length() == 13);
            EXPECT(i[12] == 'c');

            String k("fml");
            String l(" not");
            k.append(AppendVariadicFlag(), " test", l, 'c');
            EXPECT(k.length() == 13);
            EXPECT(k[0] == 'f');
            EXPECT(k[1] == 'm');
            EXPECT(k[2] == 'l');
            EXPECT(k[3] == ' ');
            EXPECT(k[4] == 't');
            EXPECT(k[5] == 'e');
            EXPECT(k[6] == 's');
            EXPECT(k[7] == 't');
            EXPECT(k[8] == ' ');
            EXPECT(k[9] == 'n');
            EXPECT(k[10] == 'o');
            EXPECT(k[11] == 't');
            EXPECT(k[12] == 'c');

            //test variadic constructor
            String m = String::concat("abc", l, 'c');
            EXPECT(m.length() == 8);
            EXPECT(m[0] == 'a');
            EXPECT(m[1] == 'b');
            EXPECT(m[2] == 'c');
            EXPECT(m[3] == ' ');
            EXPECT(m[4] == 'n');
            EXPECT(m[5] == 'o');
            EXPECT(m[6] == 't');
            EXPECT(m[7] == 'c');

            String n(12);
            EXPECT(n.capacity() == 12);
        }
        {
            //find tests
            String a("Hello World!");
            Size i = a.findIndex('o');
            EXPECT(i == 4);
            i = a.findIndex('o', 6);
            EXPECT(i == 7);
            i = a.rfindIndex('o');
            EXPECT(i == 7);
            i = a.rfindIndex('o', 5);
            EXPECT(i == 4);
            i = a.rfindIndex('z');
            EXPECT(i == String::InvalidIndex);
            i = a.findIndex('z');
            EXPECT(i == String::InvalidIndex);
            i = a.findIndex("World");
            EXPECT(i == 6);
            i = a.findIndex("World", 7);
            EXPECT(i == String::InvalidIndex);
            i = a.findIndex("World!22");
            EXPECT(i == String::InvalidIndex);
            i = a.rfindIndex("llo ");
            EXPECT(i == 2);
            i = a.rfindIndex("World", 3);
            EXPECT(i == String::InvalidIndex);
            i = a.rfindIndex("World!");
            EXPECT(i == 6);
        }
        {
            //substring tests
            String a("What's Up!");
            String b = a.sub(2, 2);
            EXPECT(b == "at");
            String c = a.sub(7);
            EXPECT(c.length() == 3);
            EXPECT(c == "Up!");
        }

        //copy tests
        {
            String a("blubb");
            String b(a);
            EXPECT(a == b);
            b = "fart";
            EXPECT(b == "fart");
            String c("weee");
            b = c;
            EXPECT(b == c);

            String mc(std::move(b));
            EXPECT(mc == c);

            mc = std::move(c);
            EXPECT(mc == "weee");
        }

        //append formatted tests
        {
            String w("Hello World!!");
            w.appendFormatted(" Wow, does this work? %i %.3f", 2567, 3.564f);
            EXPECT(w.length() == 45);
            EXPECT(w == "Hello World!! Wow, does this work? 2567 3.564");
        }

        //insertion tests
        {
            String s("aaaa");
            s.insert(0, 2, 'b');
            EXPECT(s == "bbaaaa");
            EXPECT(s.length() == 6);

            s.insert(2, 2, 'c');
            EXPECT(s == "bbccaaaa");
            EXPECT(s.length() == 8);

            s.insert(8, 2, 'd');
            EXPECT(s == "bbccaaaadd");
            EXPECT(s.length() == 10);

            s.insert(0, "dd");
            EXPECT(s == "ddbbccaaaadd");
            EXPECT(s.length() == 12);

            String str("ee");
            s.insert(3, str);
            EXPECT(s == "ddbeebccaaaadd");
            EXPECT(s.length() == 14);

            String str2("Hello");
            s.insert(3, str2, 3);
            EXPECT(s == "ddbloeebccaaaadd");
            EXPECT(s.length() == 16);

            String str3("Hello");
            s.insert(1, str2, 1, 3);
            EXPECT(s == "delldbloeebccaaaadd");
            EXPECT(s.length() == 19);

            auto it = s.begin() + 2;
            it = s.insert(it, 'O');
            EXPECT(s == "deOlldbloeebccaaaadd");
            EXPECT(*it == 'O');
            EXPECT(s.length() == 20);

            auto it2 = s.cbegin() + 4;
            auto it3 = s.insert(it2, 'A', 4);
            EXPECT(s == "deOlAAAAldbloeebccaaaadd");
            EXPECT(*it3 == 'A');
            EXPECT(s.length() == 24);

            stick::FixedArray<char, 2> fa = {'1', '2'};
            auto it4 = s.insert(s.begin(), fa.begin(), fa.end());
            EXPECT(s == "12deOlAAAAldbloeebccaaaadd");
            EXPECT(*it4 == '1');
        }

        //remove tests
        {
            String s("blubber");
            s.remove(0, 2);
            printf("%s\n", s.cString());
            EXPECT(s == "ubber");
            EXPECT(s.length() == 5);
            s.remove(2);
            EXPECT(s == "ub");
            EXPECT(s.length() == 2);

            String s2("argh");
            auto it = s2.remove(s2.begin() + 2);
            EXPECT(s2 == "arh");
            EXPECT(*it == 'h');
            it = s2.remove(s2.begin() + 2);
            EXPECT(s2 == "ar");
            EXPECT(it == s2.end());

            String s3("Hello World");
            it = s3.remove(s3.begin(), s3.begin() + 3);
            EXPECT(s3 == "lo World");
            EXPECT(*it == 'l');
            EXPECT(it == s3.begin());
            it = s3.remove(s3.begin() + 2, s3.begin() + 3);
            EXPECT(s3 == "loWorld");
            EXPECT(*it == 'W');
            it = s3.remove(s3.begin() + 4, s3.end());
            EXPECT(s3 == "loWo");
            EXPECT(it == s3.end());
        }
    },
    SUITE("String Conversion Tests")
    {
        String s = toString(Int32(99));
        EXPECT(s == "99");
        EXPECT(toString(UInt32(12)) == "12");
        EXPECT(toString(Int64(10248295198789512)) == "10248295198789512");
        EXPECT(toString(UInt64(9128589127875)) == "9128589127875");
        EXPECT(toHexString(847, 6, false, true) == "0x00034f");
        EXPECT(toHexString(Int64(847), 6, true, false) == "00034F");
        EXPECT(toHexString(-847, 0, true, false) == "-34F");
        EXPECT(toHexString(UInt64(1245135), 0, true, false) == "12FFCF");
        EXPECT(toHexString(UInt32(3), 0, true, true) == "0x3");
        EXPECT(toHexString(UInt16(9), 0, true, true) == "0x9");
        EXPECT(toHexString(UInt8(5), 0, true, true) == "0x5");
        EXPECT(toHexString(Int32(3), 0, true, true) == "0x3");
        EXPECT(toHexString(Int16(-9), 0, true, true) == "-0x9");
        EXPECT(toHexString(Int8(5), 0, true, true) == "0x5");

        EXPECT(toInt32("-3") == -3);
        EXPECT(toUInt32("99993") == 99993);
        EXPECT(toInt64("1234") == 1234);
        EXPECT(toInt64("-1234") == -1234);
        EXPECT(toInt16("-255") == -255);
    },
    SUITE("Maybe Tests")
    {
        DestructorTester::reset();
        {
            Maybe<DestructorTester> a;
        }
        EXPECT(DestructorTester::destructionCount == 0);
        {
            DestructorTester dt;
            Maybe<DestructorTester> b(dt);
            b.reset();
            EXPECT(DestructorTester::destructionCount == 1);
        }
        EXPECT(DestructorTester::destructionCount == 2);

        Maybe<String> a;
        EXPECT(!a);
        a = "test";
        EXPECT(a);
        EXPECT(*a == "test");
        a.reset();
        EXPECT(!a);

        DynamicArray<Maybe<Int32>> arr;
        arr.append(1);
        arr.append(2);
        arr.append(3);
        EXPECT(arr[0]);
        EXPECT(arr[1]);
        EXPECT(arr[2]);
        arr.resize(8);
        EXPECT(arr[0]);
        EXPECT(arr[1]);
        EXPECT(arr[2]);
        EXPECT(!arr[3]);
        EXPECT(!arr[4]);
        EXPECT(!arr[5]);
        EXPECT(!arr[6]);
        EXPECT(!arr[7]);

        DynamicArray<UniquePtr<Float32>> vv;
        vv.append(UniquePtr<Float32>(defaultAllocator().create<Float32>(3.0), defaultAllocator()));
        vv.insert(vv.end(), UniquePtr<Float32>(defaultAllocator().create<Float32>(2.0), defaultAllocator()));
    },
    SUITE("Result Tests")
    {
        Result<Int32> a(Error(ec::InvalidOperation, "bla", STICK_FILE, STICK_LINE));
        EXPECT(!a);
        Result<Int32> b(20);
        EXPECT(b);
        Result<Int32> c(b);
        EXPECT(c);
        EXPECT(b.get() == 20);
        EXPECT(c.get() == 20);
        Result<Int32> d(std::move(c));
        EXPECT(d);
        EXPECT(d.get() == 20);

        String str("test");
        Result<String &> e(str);
        EXPECT(e.ensure() == "test");

        //not a test, just to print out behavior
        ResultTestClass tc = std::move(makeResult().ensure());
    },
    SUITE("Fixed Array Tests")
    {
        FixedArray<Int32, 4> array = {4, 2, 3, 1};
        EXPECT(array[0] == 4);
        EXPECT(array[1] == 2);
        EXPECT(array[2] == 3);
        EXPECT(array[3] == 1);
        EXPECT(array.count() == 4);

        Int32 expected[] = {4, 2, 3, 1};
        Int32 i = 0;
        for (Int32 el : array)
        {
            EXPECT(expected[i] == array[i]);
            ++i;
        }

        i = 3;
        for (auto it = array.rbegin(); it != array.rend(); ++it)
        {
            EXPECT(expected[i] == *it);
            --i;
        }

        FixedArray<Int32, 4> array2 = array;
        EXPECT(array2[0] == 4);
        EXPECT(array2[1] == 2);
        EXPECT(array2[2] == 3);
        EXPECT(array2[3] == 1);
        EXPECT(array2.count() == 4);
    },
    SUITE("StaticArray Tests")
    {
        StaticArray<Int32, 4> array = {4, 2};
        EXPECT(array[0] == 4);
        EXPECT(array[1] == 2);
        EXPECT(array.count() == 2);
        EXPECT(array.append(3));
        EXPECT(array.append(1));
        EXPECT(!array.append(99));
        EXPECT(array[2] == 3);
        EXPECT(array[3] == 1);
        EXPECT(array.count() == 4);

        Int32 expected[] = {4, 2, 3, 1};
        Int32 i = 0;
        for (Int32 el : array)
        {
            EXPECT(expected[i] == array[i]);
            ++i;
        }

        i = 3;
        for (auto it = array.rbegin(); it != array.rend(); ++it)
        {
            EXPECT(expected[i] == *it);
            --i;
        }

        StaticArray<Int32, 4> array2 = array;
        EXPECT(array2[0] == 4);
        EXPECT(array2[1] == 2);
        EXPECT(array2[2] == 3);
        EXPECT(array2[3] == 1);
        EXPECT(array2.count() == 4);

        StaticArray<Variant<String, Int32, Float64>, 16> varr = {"test", 1, 2.0};
        EXPECT(varr[0].get<String>() == "test");
        EXPECT(varr[1].get<Int32>() == 1);
        EXPECT(varr[2].get<Float64>() == 2.0);
        EXPECT(varr.count() == 3);

        auto varr2 = varr;
        EXPECT(varr2[0].get<String>() == "test");
        EXPECT(varr2[1].get<Int32>() == 1);
        EXPECT(varr2[2].get<Float64>() == 2.0);
        EXPECT(varr2.count() == 3);
    },
    SUITE("DynamicArray Tests")
    {
        DynamicArray<Int32> a;
        EXPECT(a.count() == 0);
        EXPECT(a.byteCount() == 0);

        a.resize(5);
        EXPECT(a.count() == 5);
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
        EXPECT(a.count() == 6);

        EXPECT(a[5] == 10);
        EXPECT(a.last() == 10);
        EXPECT(a.first() == 0);

        a.removeLast();
        EXPECT(a.last() == 4);
        EXPECT(a.count() == 5);

        Int32 expectedResults[] = {0, 1, 2, 3, 4};
        Int32 i = 0;
        for (auto e : a)
        {
            EXPECT(e == expectedResults[i]);
            ++i;
        }

        i = a.count() - 1;
        for (auto it = a.rbegin(); it != a.rend(); ++it)
        {
            EXPECT(*it == expectedResults[i]);
            --i;
        }

        a.clear();
        EXPECT(a.count() == 0);

        DynamicArray<Int32> b;
        Int32 arr[] = {1, 2, 3, 4};
        b.insert(b.begin(), 10);
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
        EXPECT(b.count() == 9);
        Int32 expectedArr[] = {10, 11, 99, 12, 13, 1, 2, 3, 4};
        i = 0;
        for (auto v : b)
        {
            printf("DA INT %i\n", v);
            EXPECT(expectedArr[i] == v);
            i++;
        }
        EXPECT(b.count() == 9);

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

        DynamicArray<Float32> b2 = {0.5f, 0.3f, 0.1f, 0.2f};
        b2.remove(b2.begin(), b2.begin() + 2);
        EXPECT(b2.count() == 2);
        EXPECT(b2[0] == 0.1f);
        EXPECT(b2[1] == 0.2f);

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

        // {
        //     DynamicArray<DestructorTester> tt2(10);
        //     tt2.resize(20);
        // }
        // EXPECT(DestructorTester::destructionCount == 25);

        DynamicArray<Int32> ttt({1, 2, 3, 4, 5});
        EXPECT(ttt.count() == 5);
        EXPECT(ttt[0] == 1);
        EXPECT(ttt[1] == 2);
        EXPECT(ttt[2] == 3);
        EXPECT(ttt[3] == 4);
        EXPECT(ttt[4] == 5);

        DynamicArray<Int32> copy = ttt;
        EXPECT(copy.count() == 5);
        EXPECT(copy[0] == 1);
        EXPECT(copy[1] == 2);
        EXPECT(copy[2] == 3);
        EXPECT(copy[3] == 4);
        EXPECT(copy[4] == 5);

        DynamicArray<Int32> tttt = {0, 52, 1, 3};
        copy = tttt;
        EXPECT(copy.count() == 4);
        EXPECT(copy[0] == 0);
        EXPECT(copy[1] == 52);
        EXPECT(copy[2] == 1);
        EXPECT(copy[3] == 3);

        DynamicArray<Int32> mcopy = std::move(copy);
        EXPECT(mcopy.count() == 4);
        EXPECT(mcopy[0] == 0);
        EXPECT(mcopy[1] == 52);
        EXPECT(mcopy[2] == 1);
        EXPECT(mcopy[3] == 3);

        mcopy = std::move(ttt);
        EXPECT(mcopy.count() == 5);
        EXPECT(mcopy[0] == 1);
        EXPECT(mcopy[1] == 2);
        EXPECT(mcopy[2] == 3);
        EXPECT(mcopy[3] == 4);
        EXPECT(mcopy[4] == 5);

        DynamicArray<Int32> farts = {0, 52, 1, 3};
        farts.resize(8, 99);
        EXPECT(farts[0] == 0);
        EXPECT(farts[1] == 52);
        EXPECT(farts[2] == 1);
        EXPECT(farts[3] == 3);
        EXPECT(farts[4] == 99);
        EXPECT(farts[5] == 99);
        EXPECT(farts[6] == 99);
        EXPECT(farts[7] == 99);

        DynamicArray<Int32> darts = {0, 52, 1, 3, 99, 12, 18, 4};
        darts.remove(darts.begin() + 5, darts.end());
        EXPECT(darts.count() == 5);
        darts.remove(darts.begin() + 1, darts.begin() + 3);
        EXPECT(darts.count() == 3);
        EXPECT(darts[0] == 0);
        EXPECT(darts[1] == 3);
        EXPECT(darts[2] == 99);

        DynamicArray<Int32> ia = {0, 1, 2};
        DynamicArray<Int32> ib = {3, 4, 5, 6, 7, 8, 9};
        ia.insert(ia.end(), ib.begin(), ib.end());
        EXPECT(ia[0] == 0);
        EXPECT(ia[1] == 1);
        EXPECT(ia[2] == 2);
        EXPECT(ia[3] == 3);
        EXPECT(ia[4] == 4);
        EXPECT(ia[5] == 5);
        EXPECT(ia[6] == 6);
        EXPECT(ia[7] == 7);
        EXPECT(ia[8] == 8);
        EXPECT(ia[9] == 9);
        EXPECT(ia.count() == 10);

        DynamicArray<Int32> ic;
        ic.insert(ic.end(), ib.begin(), ib.end());
        EXPECT(ic[0] == 3);
        EXPECT(ic[1] == 4);
        EXPECT(ic[2] == 5);
        EXPECT(ic[3] == 6);
        EXPECT(ic[4] == 7);
        EXPECT(ic[5] == 8);
        EXPECT(ic[6] == 9);
        EXPECT(ic.count() == 7);
    },
    SUITE("Path tests")
    {
        String path = "/Absolute/Path/";
        EXPECT(path::isAbsolute(path));
        EXPECT(!path::isRelative(path));

        StringArray pathSegments = path::segments(path);
        EXPECT(pathSegments.count() == 2);
        EXPECT(pathSegments[0] == "Absolute");
        EXPECT(pathSegments[1] == "Path");

        String filePath = "../foo.txt";
        EXPECT(path::isRelative(filePath));

        path::SplitResult sp = path::split(filePath);

        EXPECT(sp.left == "..");
        EXPECT(sp.right == "foo.txt");
        EXPECT(path::directoryName(filePath) == "..");
        EXPECT(path::fileName(filePath) == "foo.txt");
        EXPECT(path::extension(filePath) == ".txt");

        path::SplitResult extsp = path::splitExtension(filePath);

        EXPECT(extsp.left == "../foo");
        EXPECT(extsp.right == ".txt");

        String jp = path::join("/Foo", "Bar");
        EXPECT(jp == "/Foo/Bar");

        StringArray segs;
        segs.append("Foo");
        segs.append("Bar");
        segs.append("Tar");

        String fs = path::fromSegments(segs);

        EXPECT(fs == "Foo/Bar/Tar");

        String fs2 = path::fromSegments(segs, true, true);

        EXPECT(fs2 == "/Foo/Bar/Tar/");

        //Path normalization
        String doubleSlashPath = "/Foo//Bar";
        EXPECT(path::normalize(doubleSlashPath) == "/Foo/Bar");

        String weirdPath = "Foo/./Bar/../bazz.jpg";
        EXPECT(path::normalize(weirdPath) == "Foo/bazz.jpg");
        String weirdPathTwo = "../Bar/./bazz.jpg";
        EXPECT(path::normalize(weirdPathTwo) == "Bar/bazz.jpg");
        EXPECT(path::normalize(weirdPathTwo, false) == "../Bar/bazz.jpg");

        //directory name
        String dirdirdir("/foo/bar/bazz");
        String dirdir = path::directoryName(dirdirdir);
        EXPECT(dirdir == "/foo/bar");
        String dir = path::directoryName(dirdir);
        EXPECT(dir == "/foo");
    },
    SUITE("URITests")
    {
        URI empty;
        EXPECT(empty.isEmpty());

        URI test("http://www.foo.org/test/index.html?bar=0#bazz");
        EXPECT(test.scheme() == "http");
        EXPECT(test.host() == "www.foo.org");
        EXPECT(test.port() == 80);
        EXPECT(test.path() == "/test/index.html");
        EXPECT(test.query() == "bar=0");
        EXPECT(test.fragment() == "bazz");
        EXPECT(test.userInfo() == "");
        EXPECT(toString(test) == "http://www.foo.org/test/index.html?bar=0#bazz");

        URI IP6URI("ftp://[::1]:1234/foo");
        EXPECT(IP6URI.scheme() == "ftp");
        EXPECT(IP6URI.host() == "::1");
        EXPECT(IP6URI.port() == 1234);
        EXPECT(IP6URI.path() == "/foo");
        EXPECT(IP6URI.query() == "");
        EXPECT(IP6URI.fragment() == "");

        URI justAPath("testPath");
        EXPECT(justAPath.isRelative());
        EXPECT(justAPath.scheme() == "");
        EXPECT(justAPath.host() == "");
        EXPECT(justAPath.port() == 0);
        EXPECT(justAPath.path() == "testPath");
        EXPECT(justAPath.query() == "");
        EXPECT(justAPath.fragment() == "");

        URI absPath("/testPath");
        EXPECT(absPath.isRelative());

        URI mailtoURI("mailto:foo@bar.com");
        EXPECT(mailtoURI.scheme() == "mailto");
        EXPECT(mailtoURI.host() == "");
        EXPECT(mailtoURI.port() == 0);
        EXPECT(mailtoURI.path() == "foo@bar.com");
        EXPECT(mailtoURI.query() == "");
        EXPECT(mailtoURI.fragment() == "");

        URI HTTPPortURI("http://foo.org:8080?test=0");
        EXPECT(HTTPPortURI.scheme() == "http");
        EXPECT(HTTPPortURI.host() == "foo.org");
        EXPECT(HTTPPortURI.port() == 8080);
        EXPECT(HTTPPortURI.path() == "");
        EXPECT(HTTPPortURI.query() == "test=0");
        EXPECT(HTTPPortURI.fragment() == "");

        URI userInfoURI("http://foo@bar.com/");
        EXPECT(userInfoURI.scheme() == "http");
        EXPECT(userInfoURI.host() == "bar.com");
        EXPECT(userInfoURI.port() == 80);
        EXPECT(userInfoURI.userInfo() == "foo");
        EXPECT(userInfoURI.path() == "/");
        EXPECT(toString(userInfoURI) == "http://foo@bar.com/");

        URI telURI("tel:+1-816-555-1212");
        EXPECT(telURI.scheme() == "tel");
        EXPECT(telURI.path() == "+1-816-555-1212");

        //a bad uri should not parse
        URI bad;
        Error err = bad.parse("ftp://[::01");
        EXPECT(err == ec::BadURI);

        URI dt("dörthe is dancing");
        String encoded = toString(dt);
        EXPECT(encoded == "d%C3%B6rthe%20is%20dancing");

        //decode it again
        URI dt2(encoded);
        //and check if the unencoded path matches the input
        EXPECT(dt2.path() == "dörthe is dancing");

        URI encodedQuery("?förde=0");
        EXPECT(encodedQuery.encodedQuery() == "f%C3%B6rde=0");

        URI encodedFragment("#förde=0");
        EXPECT(encodedFragment.encodedFragment() == "f%C3%B6rde=0");

        //check if bad encodings throw
        URI bad2;
        err = bad2.parse("http://www.foo.org/%0");
        EXPECT(err == ec::BadURI);

        URI bad3;
        err = bad3.parse("%öa");
        EXPECT(err == ec::BadURI);
    },
    SUITE("RBTree Tests")
    {
        RBTree<Int32> emptyTree;

        RBTree<Int32> tree;
        EXPECT(tree.count() == 0);

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

        EXPECT(tree.count() == 9);

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
        EXPECT(tree.count() == 6);

        auto n4 = tree.find(1);
        auto n5 = tree.find(8);
        auto n6 = tree.find(24);

        EXPECT(n4 == nullptr);
        EXPECT(n5 == nullptr);
        EXPECT(n6 == nullptr);

        //copy construct
        RBTree<String> bla;
        bla.insert("yes");
        bla.insert("no");
        bla.insert("awesome");

        auto tree2 = bla;
        auto n21 = tree2.find("yes");
        auto n22 = tree2.find("no");
        auto n23 = tree2.find("awesome");

        EXPECT(n21 != nullptr);
        EXPECT(n22 != nullptr);
        EXPECT(n23 != nullptr);
        EXPECT(n21->value == "yes");
        EXPECT(n22->value == "no");
        EXPECT(n23->value == "awesome");

        auto tree3 = std::move(tree2);
        auto n212 = tree3.find("yes");
        auto n222 = tree3.find("no");
        auto n232 = tree3.find("awesome");

        //after moving, the pointers from tree2 should now sit in tree3
        EXPECT(n21 == n212);
        EXPECT(n22 == n222);
        EXPECT(n23 == n232);

        RBTree<String> bla2;
        bla2.insert("hello");
        bla2.insert("world");

        RBTree<String> bla2Copy;
        bla2Copy = bla2;
        auto n31 = bla2Copy.find("hello");
        auto n32 = bla2Copy.find("world");
        EXPECT(n31 != nullptr);
        EXPECT(n32 != nullptr);
        EXPECT(n31->value == "hello");
        EXPECT(n32->value == "world");

        RBTree<String> bla3Copy;
        bla3Copy = std::move(bla2Copy);
        auto n312 = bla3Copy.find("hello");
        auto n322 = bla3Copy.find("world");
        EXPECT(n312 == n31);
        EXPECT(n322 == n32);
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
        EXPECT(map.count() == 1);

        auto res2 = map.insert("a", 2);
        EXPECT(res2.inserted == false);
        EXPECT(res2.iterator->value == 2);
        EXPECT(map.count() == 1);

        map.insert("b", 3);
        map.insert("c", 4);
        EXPECT(map.count() == 3);

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
        EXPECT(map.count() == 6);

        it = map.begin();

        auto lastKey = it->key;
        auto lastVal = it->value;

        it++;
        EXPECT(it->key != lastKey);
        EXPECT(it->value != lastVal);
        --it;
        EXPECT(it->key == lastKey);
        EXPECT(it->value == lastVal);


        TestMapType::KeyType * expectedKeys = new TestMapType::KeyType[map.count()];
        TestMapType::ValueType * expectedVals = new TestMapType::ValueType[map.count()];

        Size i = 0;
        for (const auto & kv : map)
        {
            expectedKeys[i] = kv.key;
            expectedVals[i] = kv.value;
            i++;
        }

        EXPECT(i == map.count());

        --i;
        Size j = 0;
        for (auto rit = map.rbegin(); rit != map.rend(); ++rit)
        {
            EXPECT(expectedKeys[i] == rit->key);
            EXPECT(expectedVals[i] == rit->value);
            ++j;
            --i;
        }

        EXPECT(j == map.count());

        delete [] expectedVals;
        delete [] expectedKeys;

        //remove tests
        it = map.find("f");
        auto it3 = it + 1;
        auto it2 = map.remove(it);
        EXPECT(it3 == it2);
        EXPECT(map.count() == 5);

        it = map.find("c");
        it++;
        it2 = map.remove("c");
        EXPECT(it == it2);
        EXPECT(map.count() == 4);

        Map<Int32, String> amap = {{3, "test"}, {2, "bla"}, {199, "blubb"}};
        EXPECT(amap.count() == 3);
        auto tit = amap.find(3);
        EXPECT(tit->key == 3);
        EXPECT(tit->value == "test");
        tit = amap.find(2);
        EXPECT(tit->key == 2);
        EXPECT(tit->value == "bla");
        tit = amap.find(199);
        EXPECT(tit->key == 199);
        EXPECT(tit->value == "blubb");
        amap.insert({{80, "wooo"}, {77, "hoo"}});
        EXPECT(amap[80] == "wooo");
        EXPECT(amap[77] == "hoo");
        EXPECT(amap.count() == 5);

        amap = {{3, "test"}, {2, "bla"}, {199, "blubb"}};
        EXPECT(amap.count() == 3);
        EXPECT(amap[3] == "test");
        EXPECT(amap[2] == "bla");
        EXPECT(amap[199] == "blubb");

        //copy tests
        Map<String, Int32> mapToCopy;
        mapToCopy.insert("hello", 1);
        mapToCopy.insert("world", 2);
        auto copy = mapToCopy;
        EXPECT(copy.count() == 2);
        EXPECT(copy["hello"] == 1);
        EXPECT(copy["world"] == 2);

        auto copy2 = std::move(copy);
        EXPECT(copy2.count() == 2);
        EXPECT(copy2["hello"] == 1);
        EXPECT(copy2["world"] == 2);

        Map<String, Int32> mapToCopy2;
        mapToCopy2.insert("a", 12);
        mapToCopy2.insert("b", 21);
        mapToCopy2.insert("c", 33);

        copy2 = mapToCopy2;
        EXPECT(copy2.count() == 3);
        EXPECT(copy2["a"] == 12);
        EXPECT(copy2["b"] == 21);
        EXPECT(copy2["c"] == 33);

        Map<String, Int32> mapToCopy3;
        mapToCopy3.insert("arr", 5);
        mapToCopy3.insert("gh", 6);

        copy2 = std::move(mapToCopy3);
        EXPECT(copy2.count() == 2);
        EXPECT(copy2["arr"] == 5);
        EXPECT(copy2["gh"] == 6);
    },
    SUITE("HashMap Tests")
    {
        HashMap<String, Int32> hm(1);

        hm.insert("test", 1);
        hm.insert("test", 2);
        EXPECT(hm.count() == 1);
        hm.insert("anotherKey", 3);
        auto res = hm.insert("blubb", 4);
        EXPECT(hm.count() == 3);
        EXPECT(res.iterator->key == "blubb");
        EXPECT(res.iterator->value == 4);
        EXPECT(res.inserted == true);

        res = hm.insert("blubb", 5);
        EXPECT(res.iterator->value == 5);
        EXPECT(res.inserted == false);
        auto it = hm.remove("anotherKey");
        EXPECT(hm.count() == 2);

        auto it2 = hm.find("blubb");
        EXPECT(it2->key == "blubb");
        EXPECT(it2->value == 5);

        //copy tests
        auto cpy = hm;
        EXPECT(cpy.count() == 2);
        EXPECT(cpy["blubb"] == 5);
        EXPECT(cpy["test"] == 2);

        HashMap<String, Int32> copyMe;
        copyMe["a"] = 1;
        copyMe["b"] = 2;
        copyMe["c"] = 3;
        auto cpied = std::move(copyMe);
        EXPECT(cpied.count() == 3);
        EXPECT(cpied["a"] == 1);
        EXPECT(cpied["b"] == 2);
        EXPECT(cpied["c"] == 3);

        //iterator tests
        Int32 counter = 0;
        for (auto & pair : cpied)
        {
            //@TODO: test iter values somehow?
            counter++;
        }
        EXPECT(counter == 3);

        cpied = cpy;
        EXPECT(cpied.count() == 2);
        EXPECT(cpied["blubb"] == 5);
        EXPECT(cpied["test"] == 2);

        HashMap<String, Int32> copyMe2;
        copyMe2["d"] = 199;
        cpy = std::move(copyMe2);
        EXPECT(cpy.count() == 1);
        EXPECT(cpy["d"] == 199);

        //initializer list tests
        HashMap<String, String> foo = {{"apple", "green"}, {"banana", "yellow"}};
        EXPECT(foo.count() == 2);
        EXPECT(foo["apple"] == "green");
        EXPECT(foo["banana"] == "yellow");

        foo.insert({{"strawberry", "red"}, {"spaceberry", "rainbow"}});
        EXPECT(foo.count() == 4);
        EXPECT(foo["apple"] == "green");
        EXPECT(foo["banana"] == "yellow");
        EXPECT(foo["strawberry"] == "red");
        EXPECT(foo["spaceberry"] == "rainbow");

        foo = {{"we", "are"}, {"done", "now"}};
        EXPECT(foo.count() == 2);
        EXPECT(foo["we"] == "are");
        EXPECT(foo["done"] == "now");

        HashMap<const void *, String> blaMap;
        blaMap[0] = "test";
        blaMap[(const void *)1] = "test2";
        EXPECT(blaMap[0] == "test");
        EXPECT(blaMap[(const void *)1] == "test2");

        // //check if the handle stuff works
        // HashMap<String, Int32> handledMap;
        // handledMap.insert("a", 2);
        // handledMap.insert("b", 3);
        // auto ires = handledMap.insert("test", 1);
        // handledMap.insert("c", 99);
        // handledMap.insert("d", 1234);

        // const HashMap<String, Int32> & cHandledMap = handledMap;
        // auto handle = ires.iterator.handle();
        // EXPECT(cHandledMap.find(handle) == ires.iterator);
        // handledMap.remove("test");
        // EXPECT(handledMap.find(handle) == handledMap.end());
        // EXPECT(handledMap.count() == 4);

        //testing if remove returns the expected iterator to the next item
        {
            HashMap<String, Int32> map;
            map.insert("test", 1);
            map.insert("test2", 2);
            map.insert("test3", 3);

            for (auto & kv : map)
            {
                printf("MAP VAL: %s %i\n", kv.key.cString(), kv.value);

            }
            auto it = map.remove("test3");
            EXPECT(it != map.end());
        }

        //check if destructors are called as expected
        {
            DestructorTester::reset();
            HashMap<Int32, UniquePtr<DestructorTester>> map;
            // printf("DES COUNT0 %i\n", DestructorTester::destructionCount);
            map.insert(1, makeUnique<DestructorTester>());
            map.insert(99, makeUnique<DestructorTester>());
            map.insert(123, makeUnique<DestructorTester>());
            // printf("DES COUNT1 %i\n", DestructorTester::destructionCount);
            map.clear();
            EXPECT(DestructorTester::destructionCount == 3);

            DestructorTester::reset();
            {
                HashMap<Int32, UniquePtr<DestructorTester>> map2;
                map2.insert(1, makeUnique<DestructorTester>());
                map2.insert(99, makeUnique<DestructorTester>());
                map2.insert(123, makeUnique<DestructorTester>());
            }
            EXPECT(DestructorTester::destructionCount == 3);

            DestructorTester::reset();
            HashMap<Int32, UniquePtr<DestructorTester>> map3;
            map3.insert(1, makeUnique<DestructorTester>());
            map3.insert(99, makeUnique<DestructorTester>());
            map3.insert(123, makeUnique<DestructorTester>());
            map3.remove(99);
            EXPECT(DestructorTester::destructionCount == 1);
        }
    },
    SUITE("Thread Tests")
    {
        Thread thread;
        std::atomic<bool> bJoinable(false);
        std::atomic<bool> bValidThreadID(false);
        Error err = thread.run([&]() { bValidThreadID = thread.threadID() != 0; bJoinable = thread.isJoinable(); Thread::sleepFor(Duration::fromSeconds(0.1f));});
        EXPECT(err.code() == 0);
        err = thread.join();
        EXPECT(bJoinable);
        EXPECT(bValidThreadID);
        EXPECT(err.code() == 0);
        EXPECT(!thread.isJoinable());

        //move test
        Thread t2;
        t2.run([&]() { Thread::sleepFor(Duration::fromSeconds(0.1f)); });
        Thread t3 = std::move(t2);
        EXPECT(t2.isJoinable() == false);
        t3.join();

        Thread t4;
        t4.run([&]() { Thread::sleepFor(Duration::fromSeconds(0.1f)); });
        t3 = std::move(t4);

        EXPECT(t4.isJoinable() == false);
        t3.join();
    },
    SUITE("ConditionVariable Tests")
    {
        Thread thread;
        ConditionVariable cond;
        Mutex m;
        bool bRun = true;
        Error err = thread.run([&]() { printf("START THREAD\n"); ScopedLock<Mutex> lock(m); while (bRun) {cond.wait(lock);} printf("END FUNC\n");});
        // auto ss = SystemClock::now();
        // auto start = HighResolutionClock::now();
        // Thread::sleepFor(Duration::fromSeconds(1.0));
        // std::cout << (HighResolutionClock::now() - start).seconds() << std::endl;
        // std::cout << (SystemClock::now() - ss).seconds() << std::endl;
        // EXPECT(err.code() == 0);
        Thread::sleepFor(Duration::fromSeconds(0.25));
        EXPECT(thread.isJoinable() == true); //the thread should still run as its blocking on the condition var
        {
            ScopedLock<Mutex> lock(m);
            bRun = false;
            err = cond.notifyOne();
        }
        EXPECT(err.code() == 0);
        printf("pre join\n");
        thread.join();
        printf("test end\n");
    },
    SUITE("FileUtilities Tests")
    {
        String uri("../../Tests/TestFiles/SomeTextFile.txt");
        auto err = saveTextFile("This is some text", uri);
        EXPECT(!err);
        auto result = loadTextFile(uri);
        EXPECT(result);
        EXPECT(result.get() == "This is some text");
        String str = result.ensure();
        EXPECT(str == "This is some text");
        auto result2 = loadTextFile("I/do/not/exist");
        EXPECT(!result2);

        String uri2("../../Tests/TestFiles/SomeBinaryData.dat");
        ByteArray data = {1, 2, 127, 3, 4};
        err = saveBinaryFile(data, uri2);
        EXPECT(!err);
        auto result3 = loadBinaryFile(uri2);
        EXPECT(result3);
        EXPECT(result3.get().count() == 5);
        EXPECT(result3.get()[0] == 1);
        EXPECT(result3.get()[1] == 2);
        EXPECT(result3.get()[2] == 127);
        EXPECT(result3.get()[3] == 3);
        EXPECT(result3.get()[4] == 4);
    },
    SUITE("TypeInfo Tests")
    {
        //test if type ID's are the same across shared library bounds
        EXPECT(TypeInfoT<Int32>::typeID() == detail::__typeIDInt32SharedLibBoundsTestFunction());
    },
    SUITE("create, destroy Tests")
    {
        Allocator & alloc = defaultAllocator();
        printf("ALLOC %lu\n", (Size)&alloc);
        auto someInt = alloc.create<Int32>(5);
        EXPECT(*someInt == 5);

        //check if the size that the create function stored for the memory allocated is correct.
        //the size is stored in the 8 bytes just before the actual pointer returned.
        // EXPECT(*reinterpret_cast<Size *>(reinterpret_cast<char *>(someInt) - sizeof(Size)) == 4);
        //EXPECT(reinterpret_cast<Allocator *>(reinterpret_cast<char *>(someInt) - sizeof(Size) - sizeof(Allocator*)) == &alloc);
        alloc.destroy(someInt);
    },
    SUITE("UniquePtr Tests")
    {
        DestructorTester::reset();
        UniquePtr<DestructorTester> a = makeUnique<DestructorTester>(defaultAllocator());
        a.reset();
        EXPECT(DestructorTester::destructionCount == 1);
        {
            UniquePtr<DestructorTester> b = makeUnique<DestructorTester>(defaultAllocator());
        }
        EXPECT(DestructorTester::destructionCount == 2);
        a = makeUnique<DestructorTester>(defaultAllocator());
        a.reset();
        EXPECT(DestructorTester::destructionCount == 3);
        {
            UniquePtr<DestructorTester> e(makeUnique<DestructorTester>(defaultAllocator()));
            UniquePtr<DestructorTester> d(std::move(e));
        }
        EXPECT(DestructorTester::destructionCount == 4);
        //TODO lots more
    },
    SUITE("FileSystem Tests")
    {
        EXPECT(fs::Permission::AllAll == static_cast<fs::Permission>(0777));
        EXPECT(fs::Permission::GroupAll == static_cast<fs::Permission>(070));
        EXPECT(fs::Permission::OwnerAll == static_cast<fs::Permission>(0700));

        EXPECT(!fs::exists("_SomeDir"));
        auto err = fs::createDirectory("_SomeDir");
        EXPECT(!err);
        EXPECT(fs::exists("_SomeDir"));
        err = fs::remove("_SomeDir");
        EXPECT(!err);
        EXPECT(!fs::exists("_SomeDir"));

        String path = "IteratorTestFolder";
        String sub = path::join(path, "Subfolder");
        String foo = path::join(path, "Foo/Bazz");
        String bar = path::join(path, "Bar");

        err = fs::removeAll(path);

        fs::createDirectory(path);
        fs::createDirectory(sub);
        fs::createDirectories(foo);
        fs::createDirectory(bar);

        int numIterations = 0;
        fs::DirectoryIterator it(path);
        //TODO: actually test if all the paths are correct
        EXPECT(!it.error());
        for (; it != fs::DirectoryIterator::End; ++it)
        {
            //printf("%s\n", it->path().cString());
            numIterations++;
        }
        EXPECT(numIterations == 3);

        fs::createDirectory(path::join(path, "empty"));

        //recursive iterator
        //TODO: actually test if all the paths are correct
        int numIterationsTwo = 0;
        fs::RecursiveDirectoryIterator rit(path);
        for (; rit != fs::RecursiveDirectoryIterator::End; ++rit)
        {
            //printf("%s\n", rit->path().cString());
            numIterationsTwo++;
        }

        EXPECT(numIterationsTwo == 5);
        err = fs::removeAll(path);
        EXPECT(!err);

        String filePath = "test.txt";
        fs::remove(filePath);
        saveTextFile("test test test", filePath);

        EXPECT(fs::exists(filePath));
        EXPECT(fs::isFile(filePath));
        EXPECT(!fs::isSymbolicLink(filePath));
        EXPECT(!fs::isDirectory(filePath));

        String symLinkPath = "testSymLink.txt";
        fs::remove(symLinkPath);
        fs::createSymbolicLink(filePath, symLinkPath);

        EXPECT(fs::readSymbolicLink(symLinkPath).ensure() == filePath);

        EXPECT(fs::exists(symLinkPath));
        EXPECT(!fs::isFile(symLinkPath));
        EXPECT(fs::isSymbolicLink(symLinkPath));
        EXPECT(!fs::isDirectory(symLinkPath));

        fs::remove(filePath);
        fs::remove(symLinkPath);
        EXPECT(!fs::exists(filePath));
        EXPECT(!fs::exists(symLinkPath));
    },
    SUITE("Variant Tests")
    {
        Variant<Float32, String> a("test");
        EXPECT(a.isValid());
        EXPECT(!a.is<Int32>());
        EXPECT(!a.is<Float32>());
        EXPECT(a.is<String>());
        EXPECT(!a.maybe<Int32>());
        EXPECT(!a.maybe<Float32>());
        EXPECT(a.maybe<String>());
        EXPECT(*a.maybe<String>() == "test");

        Variant<Float32, String> b = a;

        EXPECT(b.isValid());
        EXPECT(!b.is<Int32>());
        EXPECT(!b.is<Float32>());
        EXPECT(b.is<String>());
        EXPECT(!b.maybe<Int32>());
        EXPECT(!b.maybe<Float32>());
        EXPECT(b.maybe<String>());
        EXPECT(*b.maybe<String>() == "test");

        Variant<Float32, String> c(3.0);
        c = b;
        EXPECT(!c.is<Int32>());
        EXPECT(!c.is<Float32>());
        EXPECT(c.is<String>());
        EXPECT(!c.maybe<Int32>());
        EXPECT(!c.maybe<Float32>());
        EXPECT(c.maybe<String>());
        EXPECT(*c.maybe<String>() == "test");

        c = 1.0f;
        EXPECT(!c.is<Int32>());
        EXPECT(c.is<Float32>());
        EXPECT(!c.is<String>());
        EXPECT(!c.maybe<Int32>());
        EXPECT(c.maybe<Float32>());
        EXPECT(!c.maybe<String>());
        EXPECT(*c.maybe<Float32>() == 1.0f);

        Variant<Float32, String> d(std::move(c));
        EXPECT(d.isValid());
        EXPECT(!c.isValid());
        EXPECT(*d.maybe<Float32>() == 1.0f);

        //const tests
        Variant<const String, Int32, const Float32> e("test");
        EXPECT(e.isValid());
        EXPECT(e.is<const String>());
        EXPECT(!e.is<String>());
        EXPECT(e.get<const String>() == "test");

        const String str("blubber");
        Variant<Int32, const String *> f(&str);
        EXPECT(f.isValid());
        EXPECT(f.is<const String *>());
        EXPECT(!f.is<Int32>());
        EXPECT(*f.get<const String *>() == "blubber");

        //reference tests
        String testStr("BLUUUUB");
        Variant<bool, String &> g(testStr);
        EXPECT(g.isValid());
        EXPECT(g.is<String &>());
        EXPECT(g.get<String &>() == "BLUUUUB");
        g.get<String &>() = "changed";
        EXPECT(g.get<String &>() == "changed");

        //const ref tests
        const String testStr2("wooop");
        Variant<const String &, Float32> h(testStr2);
        EXPECT(h.isValid());
        EXPECT(h.is<const String &>());
        EXPECT(h.get<const String &>() == "wooop");
    },
    SUITE("ArgumentParser Tests")
    {
        ArgumentParser parser("Please give us all your info.");
        auto err = parser.addArgument("-h", "--help", 0, true);
        EXPECT(!err);
        err = parser.addArgument("-t", "--test", 3, false);
        EXPECT(!err);
        err = parser.addArgument("-f", "--feast", 1, false);
        EXPECT(!err);
        err = parser.addArgument("--beast", '*');
        EXPECT(!err);
        err = parser.addArgument("--almost", '+');
        EXPECT(!err);

        err = parser.addArgument([](ArgumentParser::Argument & _arg)
        {
            _arg.argCount = 2;
            _arg.shortName = "-u";
            _arg.name = "--uber";
            _arg.info = "Some info text.";
            _arg.bOptional = true;
        });
        EXPECT(!err);

        const char * args[] = {"./apps/TestApp", "--test", "1", "2", "3", "--feast", "always", "--beast", "uno", "dos", "tres"};
        err = parser.parse(args, sizeof(args) / sizeof(const char *));
        if (err)
            printf("%s\n", err.message().cString());
        EXPECT(!err);
        auto a = parser.argument("none");
        EXPECT(!a);
        auto b = parser.argument("test");
        EXPECT(b);
        EXPECT(b->values.count() == 3);
        EXPECT(b->values[0] == "1");
        EXPECT(b->values[1] == "2");
        EXPECT(b->values[2] == "3");

        auto arr = parser.get<DynamicArray<Int32>>("test");
        EXPECT(arr[0] == 1);
        EXPECT(arr[1] == 2);
        EXPECT(arr[2] == 3);

        EXPECT(parser.get<const String &>("feast") == "always");

        auto arr2 = parser.get<DynamicArray<String>>("beast");
        EXPECT(arr2[0] == "uno");
        EXPECT(arr2[1] == "dos");
        EXPECT(arr2[2] == "tres");

        printf("%s\n", parser.help().cString());
    },
    SUITE("Linear Allocator Tests")
    {
        mem::LinearAllocator<mem::Mallocator, 1024> lalloc;

        EXPECT(lalloc.block().size == 1024);

        auto a = lalloc.allocate(2048, 4);
        EXPECT(a.ptr == nullptr);
        EXPECT(a.size == 0);
        EXPECT(!lalloc.owns(a));

        auto b = lalloc.allocate(16, 4);
        EXPECT(b.ptr == lalloc.block().ptr);
        EXPECT(b.size == 16);
        EXPECT(lalloc.owns(b));

        auto c = lalloc.allocate(128, 32);
        EXPECT(reinterpret_cast<UPtr>(c.ptr) % 32 == 0);
        EXPECT(c.ptr > b.ptr);
        EXPECT(c.size == 128);
        EXPECT(lalloc.owns(c));

        lalloc.deallocateAll();
        EXPECT(lalloc.block().ptr == lalloc.currentPosition());
    },
    SUITE("Pool Allocator Tests")
    {
        mem::PoolAllocator<mem::Mallocator, 17, 32, 1024> palloc;

        auto a = palloc.allocate(16, 4);
        EXPECT(a.ptr == nullptr);

        auto a2 = palloc.allocate(33, 4);
        EXPECT(a2.ptr == nullptr);

        auto b = palloc.allocate(32, 4);
        // EXPECT(b.ptr == palloc.block().ptr);
        EXPECT(b.size == 32);

        palloc.deallocate(b);

        auto c = palloc.allocate(24, 4);
        // EXPECT(c.ptr == palloc.block().ptr);
        EXPECT(c.size == 24);

        auto d = palloc.allocate(32, 4);
        EXPECT(d.ptr > c.ptr);
        EXPECT(d.size == 32);

        palloc.deallocate(b);
        palloc.deallocate(c);
        palloc.deallocate(d);

        //Just testing if this properly compiles
        mem::PoolAllocator<mem::Mallocator, mem::DynamicSizeFlag, mem::DynamicSizeFlag, 1024> palloc2;
        palloc2.setMinMax(32, 64);
        //@TODO: More!

        {
            mem::PoolAllocator<mem::Mallocator, 0, 8, 2> palloc3;
            auto a = palloc3.allocate(4, 4);
            EXPECT(a);
            auto b = palloc3.allocate(4, 4);
            EXPECT(b);
            EXPECT(palloc3.chunkCount() == 1);
            EXPECT(palloc3.freeCount() == 0);
            auto c = palloc3.allocate(4, 4);
            EXPECT(c);
            EXPECT(palloc3.chunkCount() == 2);
            EXPECT(palloc3.freeCount() == 1);
            palloc3.deallocateAll();
            EXPECT(palloc3.freeCount() == 4);
        }
    },
    SUITE("FreeListAllocator Tests")
    {
        mem::FreeListAllocator<mem::Mallocator, 1088> falloc;

        auto a = falloc.allocate(2048, 4);
        EXPECT(!a);

        auto b = falloc.allocate(32, 4);
        EXPECT(b);
        EXPECT(b.size == 32);
        // EXPECT(b.ptr > falloc.block().ptr);
        falloc.deallocate(b);

        mem::Block blocks[4];
        for (int i = 0; i < 4; ++i)
        {
            blocks[i] = falloc.allocate(256, 4);
            EXPECT(blocks[i]);
            if (i > 0)
                EXPECT(blocks[i].ptr > blocks[i - 1].ptr);
        }

        for (int i = 0; i < 4; ++i)
        {
            EXPECT(falloc.owns(blocks[i]));
        }

        falloc.deallocate(blocks[1]);
        falloc.deallocate(blocks[3]);
        falloc.deallocate(blocks[0]);
        falloc.deallocate(blocks[2]);

        for (int i = 0; i < 4; ++i)
        {
            blocks[i] = falloc.allocate(256, 4);
            EXPECT(blocks[i]);
            if (i > 0)
                EXPECT(blocks[i].ptr > blocks[i - 1].ptr);
        }

        for (int i = 0; i < 4; ++i)
        {
            EXPECT(falloc.owns(blocks[i]));
        }

        falloc.deallocate(blocks[1]);
        falloc.deallocate(blocks[3]);
        falloc.deallocate(blocks[0]);
        falloc.deallocate(blocks[2]);

        for (int i = 0; i < 4; ++i)
        {
            blocks[i] = falloc.allocate(256, 4);
            EXPECT(blocks[i]);
            if (i > 0)
                EXPECT(blocks[i].ptr > blocks[i - 1].ptr);
        }

        for (int i = 0; i < 4; ++i)
        {
            EXPECT(falloc.owns(blocks[i]));
        }

        falloc.deallocate(blocks[0]);
        falloc.deallocate(blocks[2]);
        falloc.deallocate(blocks[1]);
        falloc.deallocate(blocks[3]);

        EXPECT(falloc.chunkCount() == 1);

        for (int i = 0; i < 4; ++i)
        {
            blocks[i] = falloc.allocate(256, 4);
            EXPECT(blocks[i]);
            if (i > 0)
                EXPECT(blocks[i].ptr > blocks[i - 1].ptr);
        }
        {
            mem::FreeListAllocator<mem::Mallocator, 512> falloc2;
            auto a = falloc2.allocate(1024, 4);
            EXPECT(!a);

            auto b = falloc2.allocate(256, 4);
            EXPECT(falloc2.freeCount() == 1);
            EXPECT(falloc2.chunkCount() == 1);
            EXPECT(b);
            EXPECT(falloc2.owns(b));

            auto c = falloc2.allocate(128, 4);
            EXPECT(falloc2.freeCount() == 1);
            EXPECT(falloc2.chunkCount() == 1);
            EXPECT(c);
            EXPECT(falloc2.owns(c));

            auto d = falloc2.allocate(128, 4);
            EXPECT(falloc2.freeCount() == 2);
            EXPECT(falloc2.chunkCount() == 2);
            EXPECT(d);
            EXPECT(falloc2.owns(d));

            falloc2.deallocateAll();
            EXPECT(falloc2.freeCount() == 2);
            EXPECT(falloc2.chunkCount() == 2);

            auto b2 = falloc2.allocate(256, 4);
            EXPECT(b2);
            EXPECT(falloc2.owns(b2));

            auto c2 = falloc2.allocate(128, 4);
            EXPECT(c2);
            EXPECT(falloc2.owns(c2));

            auto d2 = falloc2.allocate(128, 4);
            EXPECT(falloc2.freeCount() == 2);
            EXPECT(falloc2.chunkCount() == 2);
            EXPECT(d2);
            EXPECT(falloc2.owns(d2));

            mem::Block blocks[4];
            for (int i = 0; i < 4; ++i)
            {
                blocks[i] = falloc2.allocate(128, 4);
                EXPECT(blocks[i]);
            }
            for (int i = 0; i < 4; ++i)
            {
                EXPECT(falloc2.owns(blocks[i]));
            }
            EXPECT(falloc2.chunkCount() == 3);

            falloc2.deallocate(blocks[1]);
            falloc2.deallocate(blocks[3]);
            falloc2.deallocate(blocks[0]);
            falloc2.deallocate(blocks[2]);
        }
        //@TODO: More!
    },
    SUITE("Bucketizer Tests")
    {

        using PoolType = mem::PoolAllocator<mem::Mallocator,  mem::DynamicSizeFlag,  mem::DynamicSizeFlag, 256>;
        mem::Bucketizer<PoolType, 65, 128, 16> bucketizer;

        EXPECT(bucketizer.bucketCount == 4);

        auto mem = bucketizer.allocate(8, 4);
        EXPECT(!mem);
        EXPECT(!bucketizer.owns(mem));
        auto mem2 = bucketizer.allocate(65, 4);
        EXPECT(mem2);
        EXPECT(bucketizer.owns(mem2));
        auto mem3 = bucketizer.allocate(72, 4);
        EXPECT(mem3);
        EXPECT(bucketizer.owns(mem3));
        auto mem4 = bucketizer.allocate(72, 4);
        EXPECT(mem4);
        EXPECT(bucketizer.owns(mem4));
        auto mem5 = bucketizer.allocate(132, 4);
        EXPECT(!mem5);
        EXPECT(!bucketizer.owns(mem5));
        auto mem6 = bucketizer.allocate(128, 4);
        EXPECT(mem6);
        EXPECT(bucketizer.owns(mem6));

        bucketizer.deallocate(mem2);
        bucketizer.deallocate(mem3);
        bucketizer.deallocate(mem4);
        bucketizer.deallocate(mem6);

        //@TODO: More!
    },
    SUITE("Segregator Tests")
    {
        using BucketizerPoolType = mem::PoolAllocator<mem::Mallocator,  mem::DynamicSizeFlag,  mem::DynamicSizeFlag, 256>;
        using SmallAllocator = mem::PoolAllocator<mem::Mallocator, 0, 8, 1024>;
        using MediumAllocator = mem::Bucketizer<BucketizerPoolType, 9, 512, 32>;

        using Allocator = mem::Segregator <
                          mem::T<8>, SmallAllocator,
                          mem::T<512>, MediumAllocator,
                          mem::Mallocator >;

        Allocator smartAllocator;

        auto tinyChunk = smartAllocator.allocate(8, 4);
        EXPECT(tinyChunk);
        //Note: Segregator "owns" only compiles if all allocators in the chain implement it.
        //Since Mallocator does not, we directly get the allocators that we expect a chunk to be owned by
        //in order to test ownership.
        EXPECT(smartAllocator.smallAllocator().owns(tinyChunk));

        auto mediumChunk = smartAllocator.allocate(128, 4);
        EXPECT(mediumChunk);
        EXPECT(smartAllocator.largeAllocator().smallAllocator().owns(mediumChunk));

        auto largeChunk = smartAllocator.allocate(564, 4);
        EXPECT(largeChunk);
        //Note: Mallocator does not implement owns, so we won't check for it.


        smartAllocator.deallocate(tinyChunk);
        smartAllocator.deallocate(mediumChunk);
        smartAllocator.deallocate(largeChunk);

        CustomAllocator alloc;
        DynamicArray<Float32> arr(alloc);
        arr.append(1);
        arr.append(2);
        arr.append(3);
        arr.append(4);
        arr.append(5);
        arr.append(6);

        // DynamicArray<UniquePtr<Float32>> arr2(alloc);
        // arr2.append(alloc.create<Float32>(1));

        //@TODO: More!
    },
    // SUITE("Allocator Performance")
    // {
    //     using MainAllocator = mem::GlobalAllocator <
    //                           mem::Mallocator
    //                           >;

    //     // using MainAllocator = mem::Mallocator;

    //     // using MainAllocator = stick::mem::GlobalAllocator<mem::FallbackAllocator<stick::mem::FreeListAllocator<stick::mem::Mallocator, 81920000>, mem::Mallocator>>;

    //     using Palloc = mem::PoolAllocator<MainAllocator,  mem::DynamicSizeFlag,  mem::DynamicSizeFlag, 1024>;
    //     using SmallAllocator = mem::PoolAllocator<MainAllocator, 0, 8, 1024>;

    //     using SomeAllocator = mem::Segregator <
    //                           mem::T<8>, SmallAllocator,
    //                           mem::T<128>, mem::Bucketizer<Palloc, 1, 128, 16>,
    //                           mem::T<256>, mem::Bucketizer<Palloc, 129, 256, 32>,
    //                           mem::T<512>, mem::Bucketizer<Palloc, 257, 512, 64>,
    //                           mem::T<1024>, mem::Bucketizer<Palloc, 513, 1024, 128>,
    //                           mem::T<2048>, mem::Bucketizer<Palloc, 1025, 2048, 256>,
    //                           mem::T<4096>, mem::Bucketizer<Palloc, 2049, 4096, 512>,
    //                           mem::Mallocator >;

    //     SomeAllocator alloc;
    //     SystemClock clock;
    //     mem::Block blocks[1000];
    //     auto start = clock.now();
    //     for (int j = 0; j < 1000; j++)
    //     {
    //         for (int i = 0; i < 1000; ++i)
    //         {
    //             blocks[i] = alloc.allocate(32, 4);
    //         }

    //         for (int i = 0; i < 500; ++i)
    //         {
    //             alloc.deallocate(blocks[i]);
    //         }

    //         for (int i = 0; i < 500; ++i)
    //         {
    //             blocks[i] = alloc.allocate(i, 4);
    //         }

    //         for (int i = 0; i < 1000; ++i)
    //         {
    //             alloc.deallocate(blocks[i]);
    //         }

    //         // alloc.deallocateAll();
    //     }

    //     auto duration = clock.now() - start;
    //     printf("MS: %f\n", duration.milliseconds());

    //     mem::Mallocator alloc2;
    //     auto start2 = clock.now();
    //     for (int j = 0; j < 1000; j++)
    //     {
    //         for (int i = 0; i < 1000; ++i)
    //         {
    //             blocks[i] = alloc2.allocate(32, 4);
    //         }

    //         for (int i = 0; i < 500; ++i)
    //         {
    //             alloc2.deallocate(blocks[i]);
    //         }

    //         for (int i = 0; i < 500; ++i)
    //         {
    //             blocks[i] = alloc2.allocate(1024, 4);
    //         }

    //         for (int i = 0; i < 1000; ++i)
    //         {
    //             alloc2.deallocate(blocks[i]);
    //         }
    //     }
    //     auto duration2 = clock.now() - start2;
    //     printf("MS: %f\n", duration2.milliseconds());
    // }
    SUITE("Callback Tests")
    {
        //@Note: This is not really testing much but rather checking if
        //things compile as expected :)
        using Callback = stick::detail::CallbackT<void, stick::Event>;

        Callback cb(&freeFunctionCallback);

        TestClass tc;
        Callback cb2(&tc, &TestClass::memberCallback);


        bool bLamdaCalled = false;
        Callback cb3([&](const TestEvent & _evt) { bLamdaCalled = true; });

        cb.call(TestEvent());
        EXPECT(bWasCalled);

        cb2.call(TestEvent());
        EXPECT(tc.counter == 1);

        cb3.call(TestEvent());
        EXPECT(bLamdaCalled);
    },
    SUITE("EventPublisher Tests")
    {
        using EventPublisher = EventPublisherT<Event, stick::detail::PublishingPolicyBasic>;
        bWasCalled = false;
        EventPublisher publisher;

        publisher.addEventCallback(&freeFunctionCallback);
        TestClass tc;
        publisher.addEventCallback(EventPublisher::Callback(&tc, &TestClass::memberCallback));

        bool bLamdaCalled = false;
        publisher.addEventCallback([&](const TestEvent & _evt) { bLamdaCalled = true; });

        publisher.publish(TestEvent());
        EXPECT(bWasCalled);
        EXPECT(tc.counter == 1);
        EXPECT(bLamdaCalled);
    },
    SUITE("EventForwarder Tests")
    {
        using EventForwarder = EventForwarderT<Event, stick::detail::ForwardingPolicyBasic, stick::detail::PublishingPolicyBasic>;
        bWasCalled = false;
        EventForwarder publisher;

        publisher.addEventCallback(&freeFunctionCallback);
        TestClass tc;
        publisher.addEventCallback(EventForwarder::Callback(&tc, &TestClass::memberCallback));

        bool bLamdaCalled = false;
        publisher.addEventCallback([&](const TestEvent & _evt) { bLamdaCalled = true; });

        publisher.addEventFilter([&](const TestEvent & _evt) { return _evt.someMember < 128; });
        publisher.addEventModifier([&](const TestEvent & _evt) { auto ret = stick::makeUnique<TestEvent>(_evt); ret->someMember = 99; return ret; });

        EventForwarder child;
        publisher.addForwarder(child);

        int childCalledCount = 0;
        int testEvent2Called = 0;
        child.addEventCallback([&](const TestEvent2 & _evt) { testEvent2Called++; });
        child.addEventCallback([&](const TestEvent & _evt) { childCalledCount++; child.publish(TestEvent2(), true); });

        publisher.publish(TestEvent(), true);
        //this event should be filtered
        publisher.publish(TestEvent(20), true);
        EXPECT(bWasCalled);
        //check if the event modifier worked
        EXPECT(lastTestEvent.someMember == 99);
        EXPECT(tc.counter == 1);
        EXPECT(bLamdaCalled);
        EXPECT(childCalledCount == 1);
        EXPECT(testEvent2Called == 1);
    },
    SUITE("Advanced EventForwarder Tests")
    {
        //check if the passed along arguments work as expected
        using EventForwarder = EventForwarderT<Event, stick::detail::ForwardingPolicyBasic, stick::detail::PublishingPolicyBasic, stick::Int32 *>;
        bWasCalled = false;
        Int32 a = 100;
        EventForwarder publisher(stick::defaultAllocator(), &a);

        Int32 b = 27;
        EventForwarder publisher2(stick::defaultAllocator(), &b);
        publisher.addForwarder(publisher2);

        publisher.addEventCallback([](const TestEvent & _evt, Int32 * _arg)
        {
            *_arg = 54;
        });

        publisher2.addEventCallback([](const TestEvent & _evt, Int32 * _arg)
        {
            *_arg = 13;
        });

        publisher.publish(TestEvent(), true);

        EXPECT(a == 54);
        EXPECT(b == 13);
    },
    SUITE("TypeList Tests")
    {
        //@TODO: Turn all these EXPECT into static_assert?
        EXPECT(MakeTypeList<>::List::count == 0);

        using List = typename MakeTypeList<Int32, Float32, String>::List;
        EXPECT((HasType<List, Int32>::value));
        EXPECT((HasType<List, Float32>::value));
        EXPECT((HasType<List, String>::value));
        EXPECT((!HasType<List, UInt64>::value));
        EXPECT(List::count == 3);

        EXPECT((std::is_same<typename TypeAt<List, 0>::Type, Int32>::value));
        EXPECT((std::is_same<typename TypeAt<List, 1>::Type, Float32>::value));
        EXPECT((std::is_same<typename TypeAt<List, 2>::Type, String>::value));
        EXPECT((std::is_same<typename TypeAt<List, 3>::Type, TypeListNil>::value));

        using Prepend = typename PrependType<List, UInt64>::List;
        EXPECT((HasType<Prepend, Int32>::value));
        EXPECT((HasType<Prepend, Float32>::value));
        EXPECT((HasType<Prepend, String>::value));
        EXPECT((HasType<Prepend, UInt64>::value));
        EXPECT(Prepend::count == 4);

        EXPECT((std::is_same<typename TypeAt<Prepend, 0>::Type, UInt64>::value));
        EXPECT((std::is_same<typename TypeAt<Prepend, 1>::Type, Int32>::value));
        EXPECT((std::is_same<typename TypeAt<Prepend, 2>::Type, Float32>::value));
        EXPECT((std::is_same<typename TypeAt<Prepend, 3>::Type, String>::value));

        using Append = typename AppendType<List, UInt8>::List;
        EXPECT((HasType<Append, Int32>::value));
        EXPECT((HasType<Append, Float32>::value));
        EXPECT((HasType<Append, String>::value));
        EXPECT((HasType<Append, UInt8>::value));
        EXPECT(Append::count == 4);

        using AList = typename MakeTypeList<Float32, String>::List;
        using BList = typename MakeTypeList<Int32, Size>::List;
        using Merged = typename AppendTypeList<AList, BList>::List;
        EXPECT(Merged::count == 4);
        EXPECT((std::is_same<typename TypeAt<Merged, 0>::Type, Float32>::value));
        EXPECT((std::is_same<typename TypeAt<Merged, 1>::Type, String>::value));
        EXPECT((std::is_same<typename TypeAt<Merged, 2>::Type, Int32>::value));
        EXPECT((std::is_same<typename TypeAt<Merged, 3>::Type, Size>::value));
    }/*,
    SUITE("Resource Tests")
    {
        // ResourceManager manager;
        // auto result = manager.load<TestResource>("../../Tests/TestFiles/ResourceTest.txt");
        // EXPECT(result);
        // ResourceHandleT<TestResource> handle = result.get();
        // EXPECT(handle->text == "Hello World!");

        using ResourceManager = ResourceManagerT<DefaultStoragePolicy, ManualOwnershipPolicy>;
        ResourceManager manager;

        using TestResourceHandle = ResourceManager::Handle<TestResource>;
    }*/
};

int main(int _argc, const char * _args[])
{
    return runTests(spec, _argc, _args);
}
