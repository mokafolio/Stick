#ifndef STICK_ARGUMENTPARSER_HPP
#define STICK_ARGUMENTPARSER_HPP

#include <Stick/DynamicArray.hpp>
#include <Stick/HashMap.hpp>
#include <Stick/Error.hpp>
#include <Stick/Maybe.hpp>
#include <Stick/StringConversion.hpp>

namespace stick
{
    namespace detail
    {
        template<class T>
        inline T convert(const String & _str);

        template<>
        inline const String & convert<const String &>(const String & _str)
        {
            return _str;
        }

        template<>
        inline bool convert<bool>(const String & _str)
        {
            return _str == "true" || _str == "1";
        }

        template<>
        inline Float32 convert<Float32>(const String & _str)
        {
            return toFloat32(_str);
        }

        template<>
        inline Float64 convert<Float64>(const String & _str)
        {
            return toFloat64(_str);
        }

        template<>
        inline Int32 convert<Int32>(const String & _str)
        {
            return toInt32(_str);
        }

        template<>
        inline UInt32 convert<UInt32>(const String & _str)
        {
            return toUInt32(_str);
        }

        template<>
        inline Int64 convert<Int64>(const String & _str)
        {
            return toInt64(_str);
        }

        template<>
        inline UInt64 convert<UInt64>(const String & _str)
        {
            return toUInt64(_str);
        }
    }

    class STICK_API ArgumentParser
    {
    public:

        using StringArray = DynamicArray<String>;

        struct Argument
        {
            Argument(const String & _shortName, const String & _name, Int32 _count, bool _bOptional) :
            shortName(_shortName),
            name(_name),
            argCount(_count),
            bOptional(_bOptional)
            {

            }

            Argument() = default;
            Argument(const Argument &) = default;
            Argument(Argument &&) = default;
            Argument & operator = (const Argument &) = default;
            Argument & operator = (Argument &&) = default;

            template<class T>
            inline T get(Size _index)
            {
                STICK_ASSERT(_index < values.count());
                return detail::convert<T>(values[_index]);
            }

            const String & identifier() const
            {
                return shortName.length() ? shortName : name;
            }

            String shortName;
            String name;
            bool bOptional;
            Int32 argCount; //-1 = variable count
            StringArray values;
        };

        using ArgumentMap = HashMap<String, Argument>;

        ArgumentParser();

        Error addArgument(const String & _name, UInt32 _argCount, bool _bOptional = true);

        Error addArgument(const String & _shortName, const String & _name, UInt32 _argCount, bool _bOptional = true);

        Error parse(const char ** _args, UInt32 _argc);

        Maybe<const Argument &> argument(const String & _name) const;


    private:

        ArgumentMap m_args;
    };
}

#endif //STICK_ARGUMENTPARSER_HPP
