#ifndef STICK_ARGUMENTPARSER_HPP
#define STICK_ARGUMENTPARSER_HPP

#include <Stick/DynamicArray.hpp>
#include <Stick/HashMap.hpp>
#include <Stick/Error.hpp>
#include <Stick/Maybe.hpp>
#include <Stick/StringConversion.hpp>

namespace stick
{
    struct STICK_API OneOrMoreFlag {};
    struct STICK_API ZeroOrMoreFlag {};

    class STICK_API ArgumentParser
    {
    public:

        using StringArray = DynamicArray<String>;

        struct Argument
        {
            Argument(const String & _shortName, const String & _name,
                     Int32 _count, bool _bOptional, const String & _info);

            Argument();

            Argument(const Argument &) = default;
            Argument(Argument &&) = default;
            Argument & operator = (const Argument &) = default;
            Argument & operator = (Argument &&) = default;

            template<class T>
            inline T get(Size _index);

            const String & identifier() const;

            const String & printableIdentifier() const;

            String shortName;
            String name;
            bool bOptional;
            UInt8 argCount; //* = zero or more, + = one or more
            String info;
            StringArray values;
        };

        using IndexMap = HashMap<String, Size>;
        using ArgumentArray = DynamicArray<Argument>;

        ArgumentParser(const String & _info = "");

        Error addArgument(const String & _name,
                          UInt8 _argCount = 0,
                          bool _bOptional = true);

        Error addArgument(const String & _shortName,
                          const String & _name,
                          UInt8 _argCount = 0,
                          bool _bOptional = true);

        template<class F>
        inline Error addArgument(F _func);

        Error parse(const char ** _args, UInt32 _argc);

        const String & applicationName() const;

        String usage() const;

        String help() const;

        const Argument * argument(const String & _name) const;

        template<class T>
        inline Maybe<T> maybe(const String & _name) const;

        template<class T>
        inline T maybe(const String & _name, T _or) const;

        template<class T>
        inline T get(const String & _name) const;


    private:

        Error addArgumentHelper(const Argument & _arg);

        IndexMap m_indices;
        ArgumentArray m_args;
        String m_applicationName;
        String m_applicationPath;
        String m_info;
        Size m_requiredCount;
    };

    namespace detail
    {
        template<class T>
        struct ConversionHelper
        {
            static T convert(const ArgumentParser::Argument & _arg)
            {
                return convertString<T>(_arg.values[0]);
            }
        };

        template<class T>
        struct ConversionHelper<DynamicArray<T>>
        {
            static DynamicArray<T> convert(const ArgumentParser::Argument & _arg)
            {
                DynamicArray<T> ret(_arg.values.count());
                for (Size i = 0; i < ret.count(); ++i)
                {
                    ret[i] = convertString<T>(_arg.values[i]);
                }
                return ret;
            }
        };
    }

    template<class F>
    inline Error ArgumentParser::addArgument(F _func)
    {
        Argument arg;
        _func(arg);
        return addArgumentHelper(arg);
    }

    template<class T>
    inline T ArgumentParser::Argument::get(Size _index)
    {
        STICK_ASSERT(_index < values.count());
        return convertString<T>(values[_index]);
    }

    template<class T>
    inline Maybe<T> ArgumentParser::maybe(const String & _name) const
    {
        const Argument * arg = argument(_name);
        if (arg) return detail::ConversionHelper<T>::convert(*arg);
        return Maybe<T>();
    }

    template<class T>
    inline T ArgumentParser::maybe(const String & _name, T _or) const
    {
        auto m = maybe<T>(_name);
        if (m) return *m;
        return _or;
    }

    template<class T>
    inline T ArgumentParser::get(const String & _name) const
    {
        return maybe<T>(_name).value();
    }
}

#endif //STICK_ARGUMENTPARSER_HPP
