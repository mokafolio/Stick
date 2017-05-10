#include <Stick/ArgumentParser.hpp>

namespace stick
{
    namespace detail
    {
        inline Error validateName(const String & _name)
        {
            if (!_name.length())
                return Error(ec::InvalidArgument, "The argument name can't be empty", STICK_FILE, STICK_LINE);

            if (_name.length() == 2 && (_name[0] != '-' || _name.length() == 3))
                return Error(ec::InvalidArgument, "Short argument names must start with \"-\"", STICK_FILE, STICK_LINE);

            if (_name.length() > 3 && (_name[0] != '-' || _name[1] != '-'))
                return Error(ec::InvalidArgument, "Long argument names must start with \"--\"", STICK_FILE, STICK_LINE);

            return Error();
        }
    }

    ArgumentParser::ArgumentParser()
    {
    }

    Error ArgumentParser::addArgument(const String & _name, UInt32 _argCount, bool _bOptional)
    {
        Error err = detail::validateName(_name);
        if (err) return err;
        if (_name.length() > 2)
            m_args[_name] = Argument("", _name, _argCount, _bOptional);
        else
            m_args[_name] = Argument(_name, "", _argCount, _bOptional);

        return Error();
    }

    Error ArgumentParser::addArgument(const String & _shortName, const String & _name, UInt32 _argCount, bool _bOptional)
    {
        Error err = detail::validateName(_shortName);
        if (err) return err;
        err = detail::validateName(_name);
        if (err) return err;

        m_args[_shortName] = Argument(_shortName, _name, _argCount, _bOptional);

        return Error();
    }

    Error ArgumentParser::parse(const char ** _args, UInt32 _argc)
    {

    }

    Maybe<const ArgumentParser::Argument &> ArgumentParser::argument(const String & _name) const
    {

    }
}
