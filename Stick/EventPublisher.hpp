#ifndef STICK_EVENTPUBLISHER_HPP
#define STICK_EVENTPUBLISHER_HPP

#include <Stick/Private/Callback.hpp>
#include <Stick/Private/MappedCallbackStorage.hpp>
#include <Stick/Private/IndexSequence.hpp>
#include <Stick/Event.hpp>
#include <Stick/Mutex.hpp>
#include <Stick/ScopedLock.hpp>

namespace stick
{
    namespace detail
    {
        template<class PublisherType>
        struct STICK_API PublishingPolicyBasic
        {
            using MappedStorage = typename PublisherType::MappedStorage;
            using MutexType = NoMutex;
            using EventType = typename PublisherType::EventType;

            template<class...PassAlongArgs>
            inline void publish(const MappedStorage & _callbacks, const EventType & _evt, PassAlongArgs..._args)
            {
                auto it = _callbacks.callbackMap.find(_evt.eventTypeID());
                if (it != _callbacks.callbackMap.end())
                {
                    for (auto * cb : it->value)
                    {
                        cb->call(_evt, std::forward<PassAlongArgs>(_args)...);
                    }
                }
            }
            
            mutable MutexType mutex;
        };

        template<class PublisherType>
        struct STICK_API PublishingPolicyLocking
        {
            using MappedStorage = typename PublisherType::MappedStorage;
            using MutexType = Mutex;
            using EventType = typename PublisherType::EventType;

            template<class...PassAlongArgs>
            inline void publish(const MappedStorage & _callbacks, const EventType & _evt, PassAlongArgs..._args)
            {
                typename MappedStorage::RawPtrArray callbacks(_callbacks.storage.allocator());
                {
                    ScopedLock<MutexType> lck(mutex);
                    auto it = _callbacks.callbackMap.find(_evt.eventTypeID());
                    if (it != _callbacks.callbackMap.end())
                    {
                        //we copy the array here so we can savely add new callbacks from within callbacks etc.
                        callbacks = it->value;
                    }
                }

                for (auto * cb : callbacks)
                {
                    cb->call(_evt, std::forward<PassAlongArgs>(_args)...);
                }
            }

            mutable MutexType mutex;
        };
    }

    template<class EventT, template<class> class PublishingPolicyT, class...PassAlongArgs>
    class STICK_API EventPublisherT
    {
    public:

        using EventType = EventT;
        using PublishingPolicy = PublishingPolicyT<EventPublisherT>;
        using Callback = detail::CallbackT<void, EventT, PassAlongArgs...>;
        using MappedStorage = detail::MappedCallbackStorageT<typename Callback::CallbackBaseType>;
        using PassAlongArgsStorage = std::tuple<PassAlongArgs...>;

        EventPublisherT() :
            m_alloc(&defaultAllocator()),
            m_storage(defaultAllocator())
        {

        }

        EventPublisherT(Allocator & _alloc, PassAlongArgs..._args) :
            m_alloc(&_alloc),
            m_storage(_alloc),
            m_passedArgsStorage(std::forward<PassAlongArgs>(_args)...)
        {

        }

        void setPassAlongArguments(PassAlongArgs..._args)
        {
            m_passedArgsStorage = PassAlongArgsStorage(std::forward<PassAlongArgs>(_args)...);
        }

        /**
         * @brief Virtual Destructor, you usually derive from this class.
         */
        virtual ~EventPublisherT()
        {

        }

        /**
         * @brief Publish an event to all registered subscribers.
         *
         * @param _event The event to publish.
         */
        void publish(const EventType & _event)
        {
            beginPublishing(_event);
            publishImpl(_event, detail::MakeIndexSequence<sizeof...(PassAlongArgs)>());
            endPublishing(_event);
        }

        // template<class T, class...Args>
        // void publish(Args..._args)
        // {
        //     publish(makeUnique<T>(*m_alloc, _args...));
        // }

        CallbackID addEventCallback(Callback _cb)
        {
            ScopedLock<typename PublishingPolicy::MutexType> lock(m_policy.mutex);
            CallbackID id = {nextID(), _cb.eventTypeID};
            m_storage.addCallback(id, std::move(_cb.holder));
            return id;
        }

        /**
         * @brief Removes a callback.
         * @param _id The callback id to remove.
         */
        void removeEventCallback(const CallbackID & _id)
        {
            ScopedLock<typename PublishingPolicy::MutexType> lock(m_policy.mutex);
            m_storage.removeCallback(_id);
        }

        /**
         * @brief Can be overwritten if specific things need to happen right before the publisher emits its events.
         */
        virtual void beginPublishing(const EventType & _evt)
        {

        }

        /**
         * @brief Can be overwritten if specific things need to happen right after the publisher emits its events.
         */
        virtual void endPublishing(const EventType & _evt)
        {

        }


    protected:

        inline Size nextID() const
        {
            static Size s_id(0);
            return s_id++;
        }

        template<Size...S>
        inline void publishImpl(const EventType & _e, detail::IndexSequence<S...>)
        {
            m_policy.publish(m_storage, _e, std::get<S>(m_passedArgsStorage)...);
        }

        Allocator * m_alloc;
        MappedStorage m_storage;
        PassAlongArgsStorage m_passedArgsStorage;
        PublishingPolicy m_policy;
    };

    using EventPublisher = EventPublisherT<stick::Event, stick::detail::PublishingPolicyBasic>;
}

#endif //STICK_EVENTPUBLISHER_HPP
