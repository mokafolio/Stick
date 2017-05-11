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

        inline String argumentSignature(const ArgumentParser::Argument & _arg)
        {
            String ret;

            if (_arg.argCount)
            {
                String upper = detail::stripName(_arg.identifier()).toUpper();
                ret.append(" ");
                if (_arg.argCount == '*')
                {
                    ret.append("...");
                }
                else if (_arg.argCount == '+')
                {
                    ret.append(AppendVariadicFlag(), upper, " ...");
                }
                else
                {
                    for (Size i = 0; i < _arg.argCount; ++i)
                    {
                        if (i < _arg.argCount - 1)
                            ret.append(AppendVariadicFlag(), upper, " ");
                        else
                            ret.append(upper);
                    }
                }
            }

            return ret;
        }
    }

    ArgumentParser::ArgumentParser() :
        m_requiredCount(0)
    {
    }

    Error ArgumentParser::addArgument(const String & _name, UInt8 _argCount, bool _bOptional)
    {
        Error err = detail::validateName(_name);
        if (err) return err;
        if (_name.length() > 2)
            m_args[_name] = Argument("", _name, _argCount, _bOptional);
        else
            m_args[_name] = Argument(_name, "", _argCount, _bOptional);

        if (!_bOptional)
            m_requiredCount++;

        return Error();
    }

    Error ArgumentParser::addArgument(const String & _shortName, const String & _name, UInt8 _argCount, bool _bOptional)
    {
        Error err = detail::validateName(_shortName);
        if (err) return err;
        err = detail::validateName(_name);
        if (err) return err;

        m_args[_shortName] = Argument(_shortName, _name, _argCount, _bOptional);

        if (!_bOptional)
            m_requiredCount++;

        return Error();
    }

    Error ArgumentParser::parse(const char ** _args, UInt32 _argc)
    {
        // if (_argc < m_requiredCount + 1)
        //     return
    }

    String ArgumentParser::usage() const
    {
        String ret = String::concat("Usage: ", m_applicationName, " ");
        for (auto & arg : m_args)
        {
            ret.append(AppendVariadicFlag(), "[",
                       arg.value.identifier(),
                       detail::argumentSignature(arg.value), "] ");
        }
        ret.append("\n");
        return ret;
    }

    String ArgumentParser::help() const
    {
        String ret = usage();

        if (m_args.count())
        {
            if (m_requiredCount)
            {
                ret.append("\nRequired arguments:\n");
                for (auto & arg : m_args)
                {
                    if (!arg.value.bOptional)
                    {
                        if (arg.value.shortName.length())
                            ret.append(arg.value.shortName);
                        if (arg.value.name.length())
                        {
                            if (arg.value.shortName.length())
                                ret.append(", ");
                            ret.append(arg.value.name);
                        }
                        ret.append(AppendVariadicFlag(), " ", detail::argumentSignature(arg.value));
                        ret.append("\n");
                    }
                }
                ret.append("\n");
            }
            ret.append("\nOptional arguments:\n");
            for (auto & arg : m_args)
            {
                if (arg.value.bOptional)
                {
                    if (arg.value.shortName.length())
                        ret.append(arg.value.shortName);
                    if (arg.value.name.length())
                    {
                        if (arg.value.shortName.length())
                            ret.append(", ");
                        ret.append(arg.value.name);
                    }
                    ret.append(AppendVariadicFlag(), " ", detail::argumentSignature(arg.value));
                    ret.append("\n");
                }
            }
        }

        return ret;
    }

    Maybe<const ArgumentParser::Argument &> ArgumentParser::argument(const String & _name) const
    {

    }
}
