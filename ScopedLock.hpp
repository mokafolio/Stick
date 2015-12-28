#ifndef STICK_SCOPEDLOCK_HPP
#define STICK_SCOPEDLOCK_HPP

namespace stick
{
    template<class M>
    class ScopedLock
    {
    public:

        typedef M MutexType;

        ScopedLock() :
            m_bOwnsMutex(false),
            m_mutex(nullptr)
        {

        }

        ScopedLock(MutexType & _mutex) :
            m_bOwnsMutex(false),
            m_mutex(&_mutex)
        {
            Error err = m_mutex->lock();
            STICK_ASSERT(err.code() == 0);
            if (!err)
                m_bOwnsMutex = true;
        }

        Error lock()
        {
            Error err = m_mutex->lock();
            if (!err)
                m_bOwnsMutex = true;
            return err;
        }

        void unlock()
        {
            m_mutex->unlock();
            m_bOwnsMutex = false;
        }

        bool isLocked() const
        {
            return m_bOwnsMutex;
        }


    private:

        bool m_bOwnsMutex;
        MutexType * m_mutex;
    };
}

#endif //STICK_SCOPEDLOCK_HPP
