#ifndef STICK_ALLOCATORS_ALLOCATORUTILITIES_HPP
#define STICK_ALLOCATORS_ALLOCATORUTILITIES_HPP

#include <Stick/Platform.hpp>

namespace stick
{
    namespace mem
    {
        inline Size alignmentAdjustment(const void * _ptr, Size _alignment)
        {
            Size adjustment =  _alignment - ( reinterpret_cast<UPtr>(_ptr) & static_cast<UPtr>(_alignment - 1) );

            if (adjustment == _alignment)
                return 0; //already aligned

            return adjustment;
        }
    }
}

#endif //STICK_ALLOCATORS_ALLOCATORUTILITIES_HPP
