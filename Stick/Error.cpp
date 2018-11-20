#include <Stick/Error.hpp>
#include <Stick/ErrorCategory.hpp>

namespace stick
{

auto & unused = defaultAllocator();

Error::Error() : m_category(&detail::noErrorCategory()), m_code(0)
{
}

Error::Error(Int32 _code, const ErrorCategory & _category) : m_category(&_category), m_code(_code)
{
}

Error::Error(Int32 _code,
             const ErrorCategory & _category,
             const String & _message,
             const char * _file,
             UInt32 _line) :
    m_category(&_category),
    m_code(_code),
    m_message(_message),
    m_file(_file),
    m_line(_line)
{
}

Error::operator bool() const
{
    return m_category->indicatesError(*this) ? true : false;
}

bool Error::operator!() const
{
    return m_category->indicatesError(*this) ? false : true;
}

bool Error::operator==(const Error & _other) const
{
    Error a = resolvedCode();
    Error b = _other.resolvedCode();
    return a.m_code == b.m_code && a.m_category == b.m_category;
}

bool Error::operator!=(const Error & _other) const
{
    return !(*this == _other);
}

String Error::description() const
{
    return m_category->description(*this);
}

const String & Error::message() const
{
    return m_message;
}

const String & Error::file() const
{
    return m_file;
}

UInt32 Error::line() const
{
    return m_line;
}

Int32 Error::code() const
{
    return m_code;
}

Error Error::resolvedCode() const
{
    return m_category->resolveError(*this);
}

const ErrorCategory & Error::category() const
{
    return *m_category;
}
} // namespace stick
