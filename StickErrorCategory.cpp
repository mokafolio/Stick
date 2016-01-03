#include <Stick/StickErrorCategory.hpp>
#include <Stick/StickErrorCodes.hpp>
#include <string.h> //for strerror

namespace stick
{
    ErrorCategory::ErrorCategory(const String & _name) :
        m_name(_name)
    {

    }

    bool ErrorCategory::operator == (const ErrorCategory & _other) const
    {
        return this == &_other;
    }

    bool ErrorCategory::operator != (const ErrorCategory & _other) const
    {
        return this != &_other;
    }

    Error ErrorCategory::resolveError(const Error & _code) const
    {
        //the default implementation does not do anything
        return _code;
    }

    bool ErrorCategory::indicatesError(const Error & _code) const
    {
        if (_code.code() == 0)
            return false;
        return true;
    }

    const String & ErrorCategory::name() const
    {
        return m_name;
    }

    NoErrorCategory::NoErrorCategory() :
        ErrorCategory("No Error")
    {

    }

    String NoErrorCategory::description(const Error & _code) const
    {
        return "No Error";
    }

    MiscErrorCategory::MiscErrorCategory() :
        ErrorCategory("Misc")
    {

    }

    String MiscErrorCategory::description(const Error & _code) const
    {
        switch (_code.code())
        {
        case ec::InvalidOperation:
            return "Invalid operation";
            break;
        case ec::BadURI:
            return "A URI could not be parsed/created";
            break;
        case ec::NoError:
        default:
            return "No Error";
        }
    }

    SystemErrorCategory::SystemErrorCategory() :
        ErrorCategory("System")
    {

    }

    String SystemErrorCategory::description(const Error & _code) const
    {
        return String(strerror(_code.code()));
    }

    namespace detail
    {
        const NoErrorCategory & noErrorCategory()
        {
            static NoErrorCategory s_cat;
            return s_cat;
        }

        const SystemErrorCategory & systemErrorCategory()
        {
            static SystemErrorCategory s_cat;
            return s_cat;
        }

        const MiscErrorCategory & miscErrorCategory()
        {
            static MiscErrorCategory s_cat;
            return s_cat;
        }
    }
}
