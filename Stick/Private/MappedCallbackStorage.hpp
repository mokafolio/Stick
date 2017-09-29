#ifndef STICK_PRIVATE_MAPPEDCALLBACKSTORAGE_HPP
#define STICK_PRIVATE_MAPPEDCALLBACKSTORAGE_HPP

#include <Stick/CallbackID.hpp>
#include <Stick/DynamicArray.hpp>
#include <Stick/HashMap.hpp>
#include <Stick/UniquePtr.hpp>

namespace stick
{
    namespace detail
    {
        template<class CallbackBaseType>
        struct MappedCallbackStorageT
        {
            struct Storage
            {
                Size id;
                UniquePtr<const CallbackBaseType> callback;
            };

            using StorageArray = DynamicArray<Storage>;
            using RawPtrArray = DynamicArray<const CallbackBaseType *>;
            using CallbackMap = HashMap<TypeID, RawPtrArray>;

            MappedCallbackStorageT(Allocator & _alloc) :
            callbackMap(16, _alloc),
            storage(_alloc)
            {

            }

            void addCallback(const CallbackID & _cbID, const CallbackBaseType * _cb)
            {
                storage.append({_cbID.id, UniquePtr<const CallbackBaseType>(_cb, callbackMap.allocator())});
                callbackMap[_cbID.typeID].append(_cb);
            }

            void removeCallback(const CallbackID & _id)
            {
                // auto it = callbackMap.find(_id.typeID);
                // if(it != callbackMap.end())
                // {
                //     for(auto it2 = it->value.begin(); it2 != it->value.end(); ++it2)
                //     {
                //         if((*it2).id == _id.id)
                //         {
                //             it->value.remove(it2);
                //             break;
                //         }
                //     }
                //     callbackMap.remove(it);
                // }

                //@TODO: this is kinds slow since we really just linearly search...
                //is callback removal time important, though? :)

                for(auto it = storage.begin(); it != storage.end(); ++it)
                {
                    if((*it).id == _id.id)
                    {
                        auto sit = callbackMap.find(_id.typeID);
                        STICK_ASSERT(sit != callbackMap.end());
                        auto tit = find(sit->value.begin(), sit->value.end(), (*it).callback.get());
                        STICK_ASSERT(tit != sit->value.end());
                        sit->value.remove(tit);
                        if(sit->value.isEmpty())
                            callbackMap.remove(sit);
                        storage.remove(it);
                        break;
                    }
                }
            }

            CallbackMap callbackMap;
            StorageArray storage;
        };
    }
}

#endif //STICK_PRIVATE_MAPPEDCALLBACKSTORAGE_HPP
