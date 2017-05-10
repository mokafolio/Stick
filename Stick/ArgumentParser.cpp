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

        inline String stripName(const String & _name)
        {
            Size idx = 0;
            while (idx < _name.length() && _name[idx] == '-')
                idx++;
            return _name.sub(idx);
        }
    }

    ArgumentParser::ArgumentParser() :
        m_requiredCount(0)
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
        // if(_argc < m_requiredCount + 1)
        //     return
    }

    String ArgumentParser::usage() const
    {
        String ret = String::concat("Usage: ", m_applicationName, " ");
        printf("DA COUNT %lu\n", m_args.count());
        for (auto it = m_args.begin(); it != m_args.end(); ++it)
        {
            printf("APPEND BRO\n");
            ret.append(AppendVariadicFlag(), "[", it->value.identifier());
            if (it->value.argCount)
            {
                String upper = detail::stripName(it->value.identifier()).toUpper();
                ret.append(" ");
                for (Size i = 0; i < it->value.argCount; ++i)
                {
                    if(i < it->value.argCount - 1)
                        ret.append(AppendVariadicFlag(), upper, " ");
                    else
                        ret.append(upper);
                }
            }
            ret.append("]");
        }
        ret.append("\n");
        return ret;
    }

    Maybe<const ArgumentParser::Argument &> ArgumentParser::argument(const String & _name) const
    {

    }
}
