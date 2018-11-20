#ifndef STICK_PRIVATE_SINGLETON_HPP
#define STICK_PRIVATE_SINGLETON_HPP

#include <cstdlib> //for atexit

namespace stick
{
namespace detail
{

// Singletons are bad so we don't provide it as part of Sticks public API.
// That said, there are very few cases where we need to make sure that an object stays alive until
// the VERY end of the program. Within stick the only object of that nature is the DefaultAllocator
// as of now so this class only serves that singular purpose. Note: We don't use a lazy statically
// allocated singleton for this purpose, as the destruction order is not defined across shared
// library boundaries by the cpp standard. We need to do some more manual work to be able to give
// that guarantee:(.

template<class T>
class STICK_API SingletonT
{
public:

    static T & instance()
    {
        return *instancePtr();
    }

private:

    static T * instancePtr()
    {
        if(!s_instance)
        {
            s_instance = new T;
            std::atexit(destroyInstance);
        }
        return s_instance;
    }

    static void destroyInstance()
    {
        delete s_instance;
    }

    static T * s_instance;
};

template <class T>
T * SingletonT<T>::s_instance;

}
} // namespace stick

#endif // STICK_PRIVATE_SINGLETON_HPP
