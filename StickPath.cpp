#include <Stick/StickPath.hpp>

namespace stick
{
    namespace path
    {
        String separator()
        {
            return "/";
        }

        String directoryName(const String & _path)
        {

        }

        String fileName(const String & _path)
        {

        }

        String extension(const String & _path)
        {

        }

        StringArray segments(const String & _path)
        {

        }

        String fromSegments(const StringArray & _segments, bool _bAddLeadingSeparator, bool _bAddTrailingSeparator)
        {

        }

        String normalize(const String & _path, bool _bRemoveLeading)
        {

        }

        SplitResult split(const String & _path)
        {
            
        }

        SplitResult splitExtension(const String & _path)
        {

        }

        String join(const String & _a, const String & _b)
        {
            if (isAbsolute(_b))
                return _b;

            if (_a.isEmpty())
                return _b;
            else if (_b.isEmpty())
                return _a;
            else if (_a[_a.length() - 1] == '/')
                return String::concat(_a, _b);
            else
                return String::concat(_a, '/', _b);
        }

        bool isRelative(const String & _path)
        {
            if (_path[0] == '/')
                return false;
            else
                return true;
        }

        bool isAbsolute(const String & _path)
        {
            return !isRelative(_path);
        }
    }
}
