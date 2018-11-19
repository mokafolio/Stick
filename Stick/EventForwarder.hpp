#ifndef STICK_EVENTFORWARDER_HPP
#define STICK_EVENTFORWARDER_HPP

#include <Stick/EventPublisher.hpp>

namespace stick
{
namespace detail
{
template <class PublisherType>
struct STICK_API ForwardingPolicyBasic
{
    using MappedFilterStorage = typename PublisherType::MappedFilterStorage;
    using MappedModifierStorage = typename PublisherType::MappedModifierStorage;
    using MutexType = NoMutex;
    using ForwarderArray = typename PublisherType::ForwarderArray;
    using EventType = typename PublisherType::EventType;
    using EventUniquePtr = typename PublisherType::EventUniquePtr;

    template <class... PassAlongArgs>
    inline bool filter(const MappedFilterStorage & _filters,
                       const EventType & _evt,
                       PassAlongArgs... _args)
    {
        auto it = _filters.callbackMap.find(_evt.eventTypeID());
        if (it != _filters.callbackMap.end())
        {
            for (auto * cb : it->value)
            {
                if (cb->call(_evt, std::forward<PassAlongArgs>(_args)...))
                    return true;
            }
        }

        return false;
    }

    template <class... PassAlongArgs>
    inline bool filterCategory(const MappedFilterStorage & _filters,
                               const EventType & _evt,
                               PassAlongArgs... _args)
    {
        // auto it = _filters.callbackMap.find(_evt.categoryID());
        // if (it != _filters.callbackMap.end())
        // {
        //     for (auto * cb : it->value)
        //     {
        //         if (cb->call(_evt, std::forward<PassAlongArgs>(_args)...))
        //             return true;
        //     }
        // }

        for (TypeID cat : _evt.categories())
        {
            auto it = _filters.callbackMap.find(cat);
            if (it != _filters.callbackMap.end())
            {
                for (auto * cb : it->value)
                {
                    if (cb->call(_evt, std::forward<PassAlongArgs>(_args)...))
                        return true;
                }
            }
        }

        return false;
    }

    template <class... PassAlongArgs>
    inline const EventType & modify(const MappedModifierStorage & _modifiers,
                                    EventPtr & _tmpStorage,
                                    const EventType & _evt,
                                    PassAlongArgs... _args)
    {
        const EventType * ret = &_evt;
        auto it = _modifiers.callbackMap.find(_evt.eventTypeID());
        if (it != _modifiers.callbackMap.end())
        {
            for (auto * cb : it->value)
            {
                _tmpStorage = cb->call(*ret, std::forward<PassAlongArgs>(_args)...);
                ret = _tmpStorage.get();
            }
        }

        return *ret;
    }

    inline void forward(const EventType & _evt, const ForwarderArray & _forwarders)
    {
        for (auto * f : _forwarders)
        {
            f->publish(_evt, true);
        }
    }

    mutable MutexType modifierMutex;
    mutable MutexType filterMutex;
    mutable MutexType categoryFilterMutex;
    mutable MutexType forwarderMutex;
};
} // namespace detail

template <class EventT,
          template <class> class ForwardingPolicyT,
          template <class> class PublishingPolicyT,
          class... PassAlongArgs>
class STICK_API EventForwarderT
    : public EventPublisherT<EventT, PublishingPolicyT, PassAlongArgs...>
{
  public:
    using EventType = EventT;
    using EventUniquePtr = UniquePtr<EventType>;
    using ForwardingPolicy = ForwardingPolicyT<EventForwarderT>;
    using EventPublisherType = EventPublisherT<EventT, PublishingPolicyT, PassAlongArgs...>;
    using Filter = detail::CallbackT<bool, EventType, PassAlongArgs...>;
    using Modifier = detail::CallbackT<EventUniquePtr, EventType, PassAlongArgs...>;
    using ForwarderArray = DynamicArray<EventForwarderT *>;
    using MappedFilterStorage = detail::MappedCallbackStorageT<typename Filter::CallbackBaseType>;
    using MappedModifierStorage =
        detail::MappedCallbackStorageT<typename Modifier::CallbackBaseType>;

    EventForwarderT() :
        m_filterStorage(defaultAllocator()),
        m_categoryFilterStorage(defaultAllocator()),
        m_postFilterStorage(defaultAllocator()),
        m_postCategoryFilterStorage(defaultAllocator()),
        m_modifierStorage(defaultAllocator()),
        m_children(defaultAllocator())
    {
    }

    EventForwarderT(Allocator & _alloc, PassAlongArgs... _args) :
        EventPublisherType(_alloc, std::forward<PassAlongArgs>(_args)...),
        m_filterStorage(_alloc),
        m_categoryFilterStorage(_alloc),
        m_postFilterStorage(_alloc),
        m_postCategoryFilterStorage(_alloc),
        m_modifierStorage(_alloc),
        m_children(_alloc)
    {
    }

    virtual ~EventForwarderT()
    {
    }

    CallbackID addEventFilter(Filter _filter, bool _bPostPublish = false)
    {
        CallbackID id = { this->nextID(), _filter.eventTypeID };
        if (!_bPostPublish)
        {
            ScopedLock<typename ForwardingPolicy::MutexType> lock(m_forwardingPolicy.filterMutex);
            m_filterStorage.addCallback(id, std::move(_filter.holder));
        }
        else
        {
            ScopedLock<typename ForwardingPolicy::MutexType> lock(m_forwardingPolicy.filterMutex);
            m_postFilterStorage.addCallback(id, std::move(_filter.holder));
        }
        return id;
    }

    template <class Category>
    CallbackID addEventCategoryFilter(Filter _filter, bool _bPostPublish = false)
    {
        CallbackID id = { this->nextID(), TypeInfoT<Category>::typeID() };
        if (!_bPostPublish)
        {
            ScopedLock<typename ForwardingPolicy::MutexType> lock(
                m_forwardingPolicy.categoryFilterMutex);
            m_categoryFilterStorage.addCallback(id, std::move(_filter.holder));
        }
        else
        {
            ScopedLock<typename ForwardingPolicy::MutexType> lock(
                m_forwardingPolicy.categoryFilterMutex);
            m_postCategoryFilterStorage.addCallback(id, std::move(_filter.holder));
        }
        return id;
    }

    CallbackID addEventModifier(Modifier _modifier)
    {
        ScopedLock<typename ForwardingPolicy::MutexType> lock(m_forwardingPolicy.modifierMutex);
        CallbackID id = { this->nextID(), _modifier.eventTypeID };
        m_modifierStorage.addCallback(id, std::move(_modifier.holder));
        return id;
    }

    void removeEventFilter(const CallbackID & _id)
    {
        ScopedLock<typename ForwardingPolicy::MutexType> lock(m_forwardingPolicy.filterMutex);
        m_filterStorage.removeCallback(_id);
    }

    void removeEventCategoryFilter(const CallbackID & _id)
    {
        ScopedLock<typename ForwardingPolicy::MutexType> lock(
            m_forwardingPolicy.categoryFilterMutex);
        m_categoryFilterStorage.removeCallback(_id);
    }

    void removeEventModifier(const CallbackID & _id)
    {
        ScopedLock<typename ForwardingPolicy::MutexType> lock(m_forwardingPolicy.modifierMutex);
        m_modifierStorage.removeCallback(_id);
    }

    bool publish(const EventType & _evt, bool _bPropagate)
    {
        // apply pre publish filters
        if (filterAny(_evt) ||
            filterCategoryImpl(_evt,
                               m_categoryFilterStorage,
                               detail::MakeIndexSequence<sizeof...(PassAlongArgs)>()) ||
            filterImpl(
                _evt, m_filterStorage, detail::MakeIndexSequence<sizeof...(PassAlongArgs)>()))
            return false;

        EventUniquePtr tempStorage;
        const EventType & evt =
            modifyImpl(tempStorage, _evt, detail::MakeIndexSequence<sizeof...(PassAlongArgs)>());

        EventPublisherType::publish(evt);

        if (_bPropagate && !_evt.propagationStopped())
        {
            // apply post publishing filters
            if (filterCategoryImpl(_evt,
                                   m_postCategoryFilterStorage,
                                   detail::MakeIndexSequence<sizeof...(PassAlongArgs)>()) ||
                filterImpl(_evt,
                           m_postFilterStorage,
                           detail::MakeIndexSequence<sizeof...(PassAlongArgs)>()))
                return false;

            m_forwardingPolicy.forward(evt, m_children);
        }

        return true;
    }

    void addForwarder(EventForwarderT & _forwarder)
    {
        ScopedLock<typename ForwardingPolicy::MutexType> lock(m_forwardingPolicy.forwarderMutex);
        m_children.append(&_forwarder);
    }

    void removeForwarder(EventForwarderT & _forwarder)
    {
        ScopedLock<typename ForwardingPolicy::MutexType> lock(m_forwardingPolicy.forwarderMutex);
        auto it = find(m_children.begin(), m_children.end(), &_forwarder);
        if (it != m_children.end())
            m_children.remove(it);
    }

  protected:
    virtual bool filterAny(const EventType & _any)
    {
        return false;
    };

    template <Size... S>
    inline bool filterImpl(const EventType & _evt,
                           MappedFilterStorage & _filters,
                           detail::IndexSequence<S...>)
    {
        return m_forwardingPolicy.filter(_filters, _evt, std::get<S>(this->m_passedArgsStorage)...);
    }

    template <Size... S>
    inline bool filterCategoryImpl(const EventType & _evt,
                                   MappedFilterStorage & _filters,
                                   detail::IndexSequence<S...>)
    {
        return m_forwardingPolicy.filterCategory(
            _filters, _evt, std::get<S>(this->m_passedArgsStorage)...);
    }

    template <Size... S>
    inline const EventType & modifyImpl(EventUniquePtr & _tmpStorage,
                                        const EventType & _evt,
                                        detail::IndexSequence<S...>)
    {
        return m_forwardingPolicy.modify(
            m_modifierStorage, _tmpStorage, _evt, std::get<S>(this->m_passedArgsStorage)...);
    }

  private:
    MappedFilterStorage m_filterStorage;
    MappedFilterStorage m_categoryFilterStorage;
    MappedFilterStorage m_postFilterStorage;
    MappedFilterStorage m_postCategoryFilterStorage;
    MappedModifierStorage m_modifierStorage;
    ForwarderArray m_children;
    ForwardingPolicy m_forwardingPolicy;
};

using EventForwarder = EventForwarderT<stick::Event,
                                       stick::detail::ForwardingPolicyBasic,
                                       stick::detail::PublishingPolicyBasic>;
} // namespace stick

#endif // STICK_EVENTFORWARDER_HPP
