#ifndef STCK_RESOURCE_HPP
#define STCK_RESOURCE_HPP

#include <Stick/HashMap.hpp>
#include <Stick/TypeInfo.hpp>
#include <Stick/URI.hpp>
#include <Stick/UniquePtr.hpp>
#include <Stick/Result.hpp>
#include <Stick/FileUtilities.hpp>

namespace stick
{
    namespace detail
    {
        struct ResourceStorageBase
        {
            inline ResourceStorageBase() :
                referenceCount(1)
            {

            }

            virtual ~ResourceStorageBase() {}
            virtual TypeID typeID() const = 0;

            Size referenceCount;
        };

        template<class T>
        struct ResourceStorage : public ResourceStorageBase
        {
            TypeID typeID() const final
            {
                return TypeInfoT<T>::typeID();
            }

            T content;
        };

        using ResourceStorageUniquePtr = UniquePtr<ResourceStorageBase>;
        using ResourceMap = HashMap<String, detail::ResourceStorageUniquePtr>;
        using ResourceMapHandle = typename ResourceMap::Handle;
    }

    class ResourceManager;

    /*template<class T, class HT>
    class ResourceHandleT
    {
        friend class ResourceManager;

    public:

        using Type = T;

        inline ResourceHandleT() :
            m_manager(nullptr)
        {

        }

        inline ~ResourceHandleT();

        inline ResourceHandleT(const ResourceHandleT & _other);

        inline ResourceHandleT(ResourceHandleT && _other);

        inline T * operator->()
        {
            return &content();
        }

        inline const T * operator->() const
        {
            return &content();
        }

        inline T & operator*()
        {
            return content();
        }

        inline const T & operator*() const
        {
            return content();
        }

        inline T & content()
        {
            STICK_ASSERT(m_obj);
            return *m_obj;
        }

        inline const T & content() const
        {
            STICK_ASSERT(m_obj);
            return *m_obj;
        }

        inline const String & uri() const
        {

        }

    private:

        inline ResourceHandleT(ResourceManager * _manager,
                               const detail::ResourceMapHandle & _handle,
                               T * _obj) :
            m_manager(_manager),
            m_handle(_handle),
            m_obj(_obj)
        {

        }

        ResourceManager * m_manager;
        HT m_handle;
        T * m_obj;
    };*/

    // class STICK_API ResourceManager
    // {
    //     template<class T>
    //     friend class ResourceHandleT;

    // public:

    //     template<class T>
    //     class Handle
    //     {
    //     public:

    //         ResourceManager * m_manager;
    //     };

    //     inline ResourceManager(Allocator & _allocator = defaultAllocator()) :
    //         m_allocator(&_allocator),
    //         m_resources(16, _allocator)
    //     {

    //     }

    //     template<class T>
    //     inline Result<ResourceHandleT<T>> load(const String & _uri)
    //     {
    //         auto it = m_resources.find(_uri);
    //         if (it != m_resources.end())
    //         {
    //             return ResourceHandleT<T>(this, it.handle(), &static_cast<detail::ResourceStorage<T>*>(it->value.get())->content);
    //         }
    //         else
    //         {
    //             auto uptr = makeUnique<detail::ResourceStorage<T>>(*m_allocator);
    //             auto contentPtr = &uptr.get()->content;
    //             auto loadResult = loadBinaryFile(_uri, *m_allocator);
    //             if (!loadResult)
    //             {
    //                 return loadResult.error();
    //             }
    //             Error err = uptr->content.parse(&loadResult.get()[0], loadResult.get().count());
    //             if (!err)
    //             {
    //                 return ResourceHandleT<T>(this,
    //                                           m_resources.insert(_uri,
    //                                                   std::move(uptr)).iterator.handle(),
    //                                           contentPtr);
    //             }
    //             else
    //             {
    //                 return err;
    //             }
    //         }
    //     }

    //     template<class T>
    //     inline bool unload(const ResourceHandleT<T> & _resource)
    //     {

    //     }

    // private:

    //     template<class T>
    //     inline T & get(const detail::ResourceMapHandle & _handle)
    //     {
    //         auto it = m_resources.find(_handle);
    //         STICK_ASSERT(it != m_resources.end());
    //         return static_cast<detail::ResourceStorage<T>*>(it->value.get())->content;
    //     }

    //     Allocator * m_allocator;
    //     detail::ResourceMap m_resources;
    // };

    // template<class T>
    // inline ResourceHandleT<T>::~ResourceHandleT()
    // {

    // }

    // template<class T>
    // inline ResourceHandleT<T>::ResourceHandleT(const ResourceHandleT & _other)
    // {

    // }

    // template<class T>
    // inline ResourceHandleT<T>::ResourceHandleT(ResourceHandleT && _other)
    // {

    // }

    // template<class T>
    // inline T & ResourceHandleT<T>::content()
    // {
    //     STICK_ASSERT(m_manager);
    //     return m_manager->get<T>(m_handle);
    // }

    // template<class T>
    // inline const T & ResourceHandleT<T>::content() const
    // {
    //     STICK_ASSERT(m_manager);
    //     return m_manager->get<T>(m_handle);
    // }

    // template<class T>
    // inline const String & ResourceHandleT<T>::uri() const
    // {
    //     STICK_ASSERT(m_manager);
    // }

    class STICK_API ResourceManagerBase
    {
    public:

        virtual ~ResourceManagerBase()
        {

        }
    };


    class STICK_API Resource
    {
    public:

        Resource() :
            m_manager(nullptr)
        {

        }

        virtual ~Resource()
        {

        }

        const String & uri() const
        {
            return m_uri;
        }

        ResourceManagerBase & manager() const
        {
            STICK_ASSERT(m_manager);
            return *m_manager;
        }

        virtual TypeID typeID() const = 0;

    private:

        ResourceManagerBase * m_manager;
        String m_uri;
    };

    using ResourceUniquePtr = UniquePtr<Resource>;

    class StoragePolicyBase
    {
    public:

        virtual ~StoragePolicyBase() {}
    };

    template<class ResourceT>
    class DefaultStoragePolicy : public StoragePolicyBase
    {
    public:

        using ResourceType = ResourceT;
        using Map = HashMap<String, UniquePtr<ResourceT>>;
        using HandleType = typename Map::Handle;

        inline DefaultStoragePolicy(Allocator & _alloc) :
            m_map(_alloc)
        {

        }

        inline ResourceType * find(const String & _uri) const
        {
            auto it = m_map.find(_uri);
            if(it != m_map.end()) return it->value.get();
            return nullptr;
        }

        inline bool isValidHandle(const HandleType & _handle)
        {
            return _handle.bucketIndex != -1;
        }

        inline ResourceType * allocateResource(const String & _uri)
        {
            return m_map.insert(_uri, makeUnique<ResourceType>(m_map.allocator())).iterator->value.get();
        }

        // template<template<class> class HandleT, class T>
        // inline Result<HandleT<T>> load(const String & _uri)
        // {
        //     auto it = m_map.find(_uri);
        //     if (it != m_map.end())
        //     {
        //         return HandleT<T>(static_cast<T *>(it->value.get()));
        //     }
        //     else
        //     {
        //         auto uptr = makeUnique<T>(m_map.allocator());
        //         return m_map.insert()
        //     }
        // }

        Map m_map;
    };

    class ManualOwnershipPolicy
    {

    };

    template<template<class> class StoragePolicy, class HandleOwnershipPolicy>
    class STICK_API ResourceManagerT : public ResourceManagerBase
    {
    public:

        template<class T>
        class Handle
        {
        public:

            using StorageHandleType = typename StoragePolicy<T>::HandleType;
            using ResourceType = T;

            Handle() :
                m_obj(nullptr)
            {
            }

            Handle(T * _resource) :
                m_obj(_resource)
            {
                m_ownerShip.createdHandle(this);
            }

            ~Handle()
            {
                if (m_obj)
                    m_ownerShip.destroyingHandle(this);
            }

            T * m_obj;
            HandleOwnershipPolicy m_ownerShip;
        };


        template<class T>
        inline Result<Handle<T>> load(const String & _uri)
        {
            StoragePolicy<T> * s = storage<T>();
            T * resource = s->find(_uri);
            if(resource) return Handle<T>(resource);

            // auto result = loadBinaryFile(_uri, )
            // resource = s->allocateResource(_uri);
            // Error err = resource->parse()
        }

    private:

        // using StorageType = StoragePolicy;
        using StorageUniquePtr = UniquePtr<StoragePolicyBase>;
        using StorageMap = HashMap<TypeID, StorageUniquePtr>;

        template<class T>
        inline StoragePolicy<T> * storage()
        {
            auto it = m_storage.find(TypeInfoT<T>::typeID());
            if (it != m_storage.end())
                return it->value.get();
            return m_storage.insert(TypeInfoT<T>::typeID(),
                                    makeUnique<StoragePolicy<T>>(defaultAllocator())).iterator->value.get();
        }

        StorageMap m_storage;
    };
}

#endif //STICK_RESOURCE_HPP
