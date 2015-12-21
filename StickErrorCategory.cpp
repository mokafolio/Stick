#include <Stick/StickErrorCategory.hpp>

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
    
    ErrorCode ErrorCategory::resolveErrorCode(const ErrorCode & _code) const
    {
        //the default implementation does not do anything
        return _code;
    }
    
    bool ErrorCategory::indicatesError(const ErrorCode & _code) const
    {
        if(_code.code() == 0)
            return false;
        return true;
    }
    
    const String & ErrorCategory::name() const
    {
        return m_name;
    }
}
