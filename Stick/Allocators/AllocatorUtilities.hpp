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

        inline Size alignmentAdjustmentWithHeader(const void * _ptr, Size _alignment, Size _headerSize)
        {
            Size adjustment = alignmentAdjustment(_ptr, _alignment);

            if (adjustment < _headerSize)
            {
                _headerSize -= adjustment;

                //Increase adjustment to fit header
                adjustment += _alignment * (_headerSize / _alignment);

                if (_headerSize % _alignment > 0)
                    adjustment += _alignment;
            }
            return adjustment;
        }
    }
}

#endif //STICK_ALLOCATORS_ALLOCATORUTILITIES_HPP
