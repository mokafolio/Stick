#ifndef STICK_EVENT_HPP
#define STICK_EVENT_HPP

#include <Stick/TypeInfo.hpp>
#include <Stick/UniquePtr.hpp>
#include <Stick/String.hpp>

namespace stick
{
    struct STICK_API DefaultEventCategory {};

    /**
     * @brief Base class for all events dispatched and received by EventPublisher and EventSubscriber.
     * @see EventT
     */
    class STICK_API Event
    {
    public:

        /**
         * @brief Default constructor.
         */
        Event();

        /**
         * @brief Virtual destructor.
         */
        virtual ~Event();

        /**
         * @brief Returns the name of the Event (mainly for logging purposes).
         *
         * The default implementation
         * simply returns the rtti name from c++ typeid. Can be overwritten.
         */
        virtual String name();

        /**
         * @brief Returns a run time identifier that uniquely identifies the type of the event.
         */
        virtual TypeID eventTypeID() const = 0;

        /**
        * @brief Returns a run time identifier that uniquely identifies the type of the event.
        */
        virtual TypeID categoryID() const = 0;

        // @TODO: This is kinda weird in terms of const correctness
        // as we pass events by const reference. At the same time its fine hehehehehehehehe
        void stopPropagation() const;

        bool propagationStopped() const;

    private:

        mutable bool m_bStopPropagation;
    };

    typedef UniquePtr<Event> EventPtr;


    /**
     * @brief Templated helper class that implements the eventTypeID function and adds some typedefs for the Event of
     * type T.
     *
     * Usually you derive from this class, rather than from Event directly.
     */
    template<class T, class Cat = DefaultEventCategory>
    class STICK_API EventT : public Event
    {
    public:

        using Category = Cat;

        /**
         * @brief The TypeInfo of T.
         */
        using TypeInfo = TypeInfoT<T>;


        /**
         * @brief The TypeInfo of the category flag.
         */
        using CategoryInfo = TypeInfoT<Category>;

        /**
         * @brief The std::shared_ptr of T.
         */
        using EventPtr = UniquePtr<T>;

        /**
         * @brief Returns the unique identifier of T.
         */
        TypeID eventTypeID() const override
        {
            return TypeInfo::typeID();
        }

        TypeID categoryID() const override
        {
            return CategoryInfo::typeID();
        }
    };
}

#endif //STICK_EVENT_HPP
