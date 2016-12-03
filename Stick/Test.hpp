#ifndef STICK_TEST_HPP
#define STICK_TEST_HPP

#include <Stick/DynamicArray.hpp>
#include <Stick/String.hpp>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define SUITE(_title) \
    _title, [](stick::detail::TestSession & _session)

#define EXPECT(_expr) \
    do \
    { \
        _session.testCount++; \
        if(_expr) \
        { \
            printf(ANSI_COLOR_GREEN "Line: %u, %s, OK\n" ANSI_COLOR_RESET, STICK_LINE, #_expr); \
        } \
        else \
        { \
            _session.failCount++; \
            printf(ANSI_COLOR_RED "Line: %u, %s, Failed\n" ANSI_COLOR_RESET, STICK_LINE, #_expr); \
        } \
    } while(false)


namespace stick
{
    namespace detail
    {
        struct TestSession
        {
            TestSession() :
            failCount(0),
            testCount(0)
            {

            }

            Size failCount;
            Size testCount;
        };

        struct TestFunctionHolder
        {
            virtual ~TestFunctionHolder(){}
            virtual void call(detail::TestSession & _s) = 0;
            virtual TestFunctionHolder * clone() const = 0;
        };

        template<class F>
        struct TestFunctorHolder : public TestFunctionHolder
        {
            TestFunctorHolder(F _f) :
            functor(_f)
            {

            }

            void call(detail::TestSession & _s) override
            {
                functor(_s);
            }

            TestFunctionHolder * clone() const override
            {
                return new TestFunctorHolder(*this);
            }

            F functor;
        };

        struct TestFunction
        {
            template<class F>
            TestFunction(F _f) :
            function(new TestFunctorHolder<F>(_f))
            {
            }

            ~TestFunction()
            {
                if(function)
                    delete function;
            }

            TestFunction(const TestFunction & _other) :
            function(nullptr)
            {
                if(_other.function)
                    function = _other.function->clone();
            }

            TestFunction(TestFunction && _other) :
            function(std::move(_other.function))
            {
                _other.function = nullptr;
            }

            void operator()(TestSession & _session) const
            {
                if(function)
                    function->call(_session);
            }

            TestFunctionHolder * function;
        };
    }

    struct Suite
    {
        String title;
        detail::TestFunction function;
    };

    template<Size N>
    inline int runTests(const Suite (&_spec)[N], int _argc, const char * _args[])
    {
        //args ignored for now

        detail::TestSession session;
        Size failedSuites = 0;
        for(Size i=0; i < N; ++i)
        {
            session.failCount = 0;
            session.testCount = 0;
            printf(ANSI_COLOR_CYAN "Running %s\n", _spec[i].title.cString());
            printf("================================\n");
            _spec[i].function(session);
            printf(ANSI_COLOR_CYAN "%lu / %lu Tests passed.\n\n", session.testCount - session.failCount, session.testCount);
            if(session.failCount)
                failedSuites++;
        }
        printf(ANSI_COLOR_YELLOW "%lu/%lu test suites passed.\n" ANSI_COLOR_RESET, N-failedSuites, N);

        return failedSuites ? EXIT_FAILURE : EXIT_SUCCESS;
    }
}

#endif //STICK_TEST_HPP
