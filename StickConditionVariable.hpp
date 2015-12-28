#ifndef STICK_CONDITIONVARIABLE_HPP
#define STICK_CONDITIONVARIABLE_HPP

namespace stick
{
    class Mutex;

    class ConditionVariable
    {
    public:

#ifdef STICK_PLATFORM_UNIX
        typedef pthread_cond_t NativeHandle;
#endif //STICK_PLATFORM_UNIX

        void notifyOne();

        void notifyAll();

        void wait(Mutex & _mutex);

        NativeHandle nativeHandle() const;

    private:

        NativeHandle m_handle;
    };
}

#endif //STICK_CONDITIONVARIABLE_HPP
