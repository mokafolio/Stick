#ifndef STICK_EVENT_HPP
#define STICK_EVENT_HPP

#include <Stick/StaticArray.hpp>
#include <Stick/String.hpp>
#include <Stick/TypeInfo.hpp>
#include <Stick/TypeList.hpp>
#include <Stick/UniquePtr.hpp>

#define STICK_MAX_EVENT_CATEGORIES 8

namespace stick
{
struct STICK_API DefaultEventCategory
{
};

using CategoryIDArray = StaticArray<TypeID, STICK_MAX_EVENT_CATEGORIES>;

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
     * @brief Returns if the Event is in the provided category
     */
    virtual bool hasCategory(TypeID _id) const = 0;

    /**
     * @brief Returns an array of category ids that the event type is part of
     */
    virtual CategoryIDArray categories() const = 0;

    // @TODO: This is kinda weird in terms of const correctness
    // as we pass events by const reference. At the same time its fine hehehehehehehehe
    void stopPropagation() const;

    bool propagationStopped() const;

  private:
    mutable bool m_bStopPropagation;
};

typedef UniquePtr<Event> EventPtr;

namespace detail
{
template <class L>
struct CategoryCompHelper
{
    static bool isCategory(TypeID _cat)
    {
        if (TypeInfoT<typename L::Head>::typeID() == _cat)
            return true;
        else
            return CategoryCompHelper<typename L::Tail>::isCategory(_cat);
    }
};

template <>
struct CategoryCompHelper<stick::TypeListNil>
{
    static bool isCategory(TypeID _cat)
    {
        return false;
    }
};

template <class TL>
struct CategoryComp
{
    static bool isCategory(TypeID _cat)
    {
        return CategoryCompHelper<TL>::isCategory(_cat);
    }
};
} // namespace detail

/**
 * @brief Templated helper class that implements the eventTypeID function and adds some typedefs for
 * the Event of type T.
 *
 * Usually you derive from this class, rather than from Event directly.
 */
template <class T, class Cat = DefaultEventCategory, class... Cats>
class STICK_API EventT : public Event
{
  public:
    /**
     * @brief The categories that this event type is part of.
     */
    using Categories = typename MakeTypeList<Cat, Cats...>::List;

    /**
     * @brief The TypeInfo of T.
     */
    using TypeInfo = TypeInfoT<T>;

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

    bool hasCategory(TypeID _id) const override
    {
        return detail::CategoryComp<Categories>::isCategory(_id);
    }

    CategoryIDArray categories() const override
    {
        return { TypeInfoT<Cat>::typeID(), TypeInfoT<Cats>::typeID()... };
    }
};
} // namespace stick

#endif // STICK_EVENT_HPP
