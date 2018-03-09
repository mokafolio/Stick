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
            virtual ~ResourceStorageBase() {}
            virtual TypeID typeID() const = 0;
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
        using ResourceMap = HashMap<URI, detail::ResourceStorageUniquePtr>;
        using ResourceHandle = typename ResourceMap::Handle;
    }

    class ResourceManager;

    template<class T>
    class ResourceT
    {
        friend class ResourceManager;

    public:

        using Type = T;

        inline ResourceT() :
            m_manager(nullptr)
        {

        }

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

        inline T & content();

        inline const T & content() const;

        inline const String & uri() const;

    private:

        inline ResourceT(ResourceManager * _manager, const detail::ResourceHandle & _handle) :
            m_manager(_manager),
            m_handle(_handle)
        {

        }

        ResourceManager * m_manager;
        detail::ResourceHandle m_handle;
    };

    class STICK_API ResourceManager
    {
        template<class T>
        friend class ResourceT;

    public:

        inline ResourceManager(Allocator & _allocator = defaultAllocator()) :
            m_allocator(&_allocator),
            m_resources(16, _allocator)
        {

        }

        template<class T>
        inline Result<ResourceT<T>> load(const URI & _uri)
        {
            //@TODO: pass in allocator to toString as soon as that works
            String struri = toString(_uri);
            auto it = m_resources.find(_uri);
            if (it != m_resources.end())
            {
                return ResourceT<T>(this, it.handle());
            }
            else
            {
                auto uptr = makeUnique<detail::ResourceStorage<T>>(*m_allocator);
                auto loadResult = loadBinaryFile(_uri, *m_allocator);
                if (!loadResult)
                {
                    return loadResult.error();
                }
                Error err = uptr->content.parse(&loadResult.get()[0], loadResult.get().count());
                if (!err)
                {
                    return ResourceT<T>(this,
                                        m_resources.insert(_uri,
                                                           std::move(uptr)).iterator.handle());
                }
                else
                {
                    return err;
                }
            }
        }

    private:

        template<class T>
        inline T & get(const detail::ResourceHandle & _handle)
        {
            auto it = m_resources.find(_handle);
            STICK_ASSERT(it != m_resources.end());
            return static_cast<detail::ResourceStorage<T>*>(it->value.get())->content;
        }

        Allocator * m_allocator;
        detail::ResourceMap m_resources;
    };

    template<class T>
    inline T & ResourceT<T>::content()
    {
        STICK_ASSERT(m_manager);
        return m_manager->get<T>(m_handle);
    }

    template<class T>
    inline const T & ResourceT<T>::content() const
    {
        STICK_ASSERT(m_manager);
        return m_manager->get<T>(m_handle);
    }
}

#endif //STICK_RESOURCE_HPP
