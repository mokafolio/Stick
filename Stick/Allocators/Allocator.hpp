#ifndef STICK_ALLOCATORS_ALLOCATOR_HPP
#define STICK_ALLOCATORS_ALLOCATOR_HPP

#include <Stick/Platform.hpp>

namespace stick
{
    namespace mem
    {
        struct STICK_API Allocator
        {
            virtual void * allocate(Size _byteCount, Size _alignment) = 0;

            virtual void deallocate(void * _ptr) = 0;

            template<class T>
            inline constexpr Size defaultAllocationSizeForType()
            {
                Size ret = sizeof(T);
                return ret % alignof(T) == 0 ? ret : ret + alignof(T) - ret % alignof(T);
            }

            template<class T, class...Args>
            inline T * create(Args && ..._args)
            {
                return new (allocate(sizeof(T), alignof(T)))(_args...);
            }

            template<class T>
            inline void destroy(T * _obj)
            {
                _obj->~T();
                deallocate(reinterpret_cast<void *>(_obj));
            }
        };
    }
}

#endif //STICK_ALLOCATORS_ALLOCATOR_HPP
