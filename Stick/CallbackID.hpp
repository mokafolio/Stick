#ifndef STICK_CALLBACKID_HPP
#define STICK_CALLBACKID_HPP

#include <Stick/TypeInfo.hpp>

namespace stick
{
    struct STICK_API CallbackID
    {
        Size id;
        TypeID typeID;
    };
}

#endif //STICK_CALLBACKID_HPP
