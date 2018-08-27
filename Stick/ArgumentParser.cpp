#include <Stick/ArgumentParser.hpp>
#include <Stick/Path.hpp>

namespace stick
{
    namespace detail
    {
        inline Error validateName(const String & _name)
        {
            if (!_name.length())
                return Error(ec::InvalidArgument, "The argument name can't be empty", STICK_FILE, STICK_LINE);

            if (_name.length() < 2 && (_name[0] != '-' || _name.length() == 3))
                return Error(ec::InvalidArgument, "Short argument names must start with \"-\"", STICK_FILE, STICK_LINE);

            if (_name.length() > 2 && (_name[0] != '-' || _name[1] != '-'))
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

        inline String delimitName(const String & _name)
        {
            if (!_name.length() || _name[0] == '-') return _name;
            if (_name.length() > 1)
                return String::concat("--", _name);
            else
                return String::concat("-", _name);
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

    ArgumentParser::Argument::Argument() :
        bOptional(true),
        argCount(0),
        bArgumentWasProvided(false)
    {

    }

    ArgumentParser::Argument::Argument(const String & _shortName, const String & _name,
                                       Int32 _count, bool _bOptional, const String & _info) :
        shortName(_shortName),
        name(_name),
        argCount(_count),
        bOptional(_bOptional),
        info(_info),
        bArgumentWasProvided(false)
    {

    }

    const String & ArgumentParser::Argument::identifier() const
    {
        return shortName.length() ? shortName : name;
    }

    const String & ArgumentParser::Argument::printableIdentifier() const
    {
        return name.length() ? name : shortName;
    }


    ArgumentParser::ArgumentParser(const String & _info) :
        m_requiredCount(0),
        m_info(_info)
    {
        addArgument("-h", "--help", 0, true);
    }

    Error ArgumentParser::addArgument(const String & _name, UInt8 _argCount, bool _bOptional)
    {
        if (_name.length() > 2)
            return addArgumentHelper(Argument("", _name, _argCount, _bOptional, ""));
        else
            return addArgumentHelper(Argument(_name, "", _argCount, _bOptional, ""));
    }

    Error ArgumentParser::addArgument(const String & _shortName, const String & _name, UInt8 _argCount, bool _bOptional)
    {
        return addArgumentHelper(Argument(_shortName, _name, _argCount, _bOptional, ""));
    }

    Error ArgumentParser::addArgumentHelper(const Argument & _arg)
    {
        if (_arg.shortName.length() || _arg.name.length())
        {
            Error err;
            if (_arg.shortName.length())
            {
                err = detail::validateName(_arg.shortName);
                if (err) return err;
            }
            if (_arg.name.length())
            {
                err = detail::validateName(_arg.name);
                if (err) return err;
            }
        }
        else
        {
            return Error(ec::InvalidArgument, "An Argument needs either a short name and/or a long name.", STICK_FILE, STICK_LINE);
        }

        Size idx = m_args.count();
        m_args.append(_arg);

        if (!_arg.shortName.isEmpty())
            m_indices[_arg.shortName] = idx;
        if (!_arg.name.isEmpty())
            m_indices[_arg.name] = idx;
        if (!_arg.bOptional)
            m_requiredCount++;

        return Error();
    }

    Error ArgumentParser::parse(const char ** _args, UInt32 _argc)
    {
        if (_argc < 1)
            return Error(ec::InvalidArgument, "No arguments", STICK_FILE, STICK_LINE);

        m_applicationPath = _args[0];
        m_applicationName = path::fileName(m_applicationPath);

        Argument * active = nullptr;
        const char * activeName = nullptr;
        UInt8 activeArgTargetCount = 0;
        UInt8 currentCount = 0;
        for (Size i = 1; i < _argc; ++i)
        {
            if (std::strcmp(_args[i], "--help") == 0)
            {
                printf("%s\n", help().cString());
                std::exit(EXIT_SUCCESS);
            }

            auto it = m_indices.find(_args[i]);

            //check if this a key
            if (it != m_indices.end())
            {
                //if there is an active key, check if all its arguments are satisfied.
                if (active)
                {
                    if (activeArgTargetCount == '+' && currentCount < 1)
                    {
                        String err;
                        err.appendFormatted("Expected at least 1 argument for %s, got %i.", activeName, currentCount);
                        return Error(ec::InvalidArgument, err, STICK_FILE, STICK_LINE);
                    }
                    else if ((activeArgTargetCount != '*' && activeArgTargetCount != '+' && currentCount < activeArgTargetCount) ||
                             (activeArgTargetCount == '+' && currentCount < 1))
                    {
                        String err;
                        err.appendFormatted("Expected %i arguments for %s, got %i.", activeArgTargetCount, activeName, currentCount);
                        return Error(ec::InvalidArgument, err, STICK_FILE, STICK_LINE);
                    }
                }
                active = &m_args[it->value];
                active->bArgumentWasProvided = true;
                activeName = _args[i];
                activeArgTargetCount = active->argCount;
                currentCount = 0;

                //check if there is enough args left to parse
                auto left = _argc - i - 1;
                auto tc = activeArgTargetCount == '+' ? 1 : activeArgTargetCount;
                if ((activeArgTargetCount == '+' && left < 1) || (activeArgTargetCount != '*' && activeArgTargetCount != '+' && left < activeArgTargetCount))
                {
                    String err;
                    err.appendFormatted("Expected %i arguments for %s, there are only %i left to parse.", tc, activeName, left);
                    return Error(ec::InvalidArgument, err, STICK_FILE, STICK_LINE);
                }
            }
            else if (active)
            {
                if (activeArgTargetCount != '*' && activeArgTargetCount != '+' && currentCount > activeArgTargetCount)
                {
                    String err;
                    err.appendFormatted("Expected %i arguments for %s, got %i.", activeArgTargetCount, activeName, currentCount);
                    return Error(ec::InvalidArgument, err, STICK_FILE, STICK_LINE);
                }
                active->values.append(_args[i]);
                currentCount++;
            }
        }

        //check if all required args were set.
        for (auto & arg : m_args)
        {
            if (!arg.bOptional && arg.argCount != '*' && arg.argCount > 0)
            {
                if (!arg.values.count())
                    return Error(ec::InvalidArgument, String::concat("Required argument ", arg.printableIdentifier(), " is missing."), STICK_FILE, STICK_LINE);
            }
        }

        return Error();
    }

    bool ArgumentParser::argumentWasProvided(const String & _name) const
    {
        if (auto arg = argument(_name))
        {
            return arg->bArgumentWasProvided;
        }
        return false;
    }

    String ArgumentParser::usage() const
    {
        String ret = String::concat("Usage:\n./", m_applicationName, " ");
        for (auto & arg : m_args)
        {
            ret.append(AppendVariadicFlag(), "[",
                       arg.identifier(),
                       detail::argumentSignature(arg), "] ");
        }
        ret.append("\n");
        return ret;
    }

    String ArgumentParser::help() const
    {
        String ret = usage();

        if (m_info.length())
            ret.append(AppendVariadicFlag(), "\n", m_info, "\n");

        if (m_args.count())
        {
            if (m_requiredCount)
            {
                ret.append("\nRequired arguments:\n");
                for (auto & arg : m_args)
                {
                    if (!arg.bOptional)
                    {
                        if (arg.shortName.length())
                            ret.append(arg.shortName);
                        if (arg.name.length())
                        {
                            if (arg.shortName.length())
                                ret.append(", ");
                            ret.append(arg.name);
                        }
                        ret.append(AppendVariadicFlag(), " ", detail::argumentSignature(arg), "    ", arg.info.length() ? arg.info : "", "\n");
                    }
                }
                ret.append("\n");
            }
            if (m_args.count() > m_requiredCount)
            {
                ret.append("Optional arguments:\n");
                for (auto & arg : m_args)
                {
                    if (arg.bOptional)
                    {
                        if (arg.shortName.length())
                            ret.append(arg.shortName);
                        if (arg.name.length())
                        {
                            if (arg.shortName.length())
                                ret.append(", ");
                            ret.append(arg.name);
                        }
                        ret.append(AppendVariadicFlag(), " ", detail::argumentSignature(arg), "    ", arg.info.length() ? arg.info : "", "\n");
                    }
                }
            }
        }

        return ret;
    }

    const ArgumentParser::Argument * ArgumentParser::argument(const String & _name) const
    {
        auto it = m_indices.find(detail::delimitName(_name));
        if (it != m_indices.end())
        {
            return &m_args[it->value];
        }

        return nullptr;
    }
}
