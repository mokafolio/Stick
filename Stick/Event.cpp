#include <Stick/Event.hpp>
#include <typeinfo>

namespace stick
{
    Event::Event() :
        m_bStopPropagation(false)
    {

    }

    Event::~Event()
    {

    }

    String Event::name()
    {
        return String(typeid(*this).name());
    }

    void Event::stopPropagation() const
    {
        m_bStopPropagation = true;
    }

    bool Event::propagationStopped() const
    {
        return m_bStopPropagation;
    }
}
