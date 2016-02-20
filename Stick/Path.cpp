#include <Stick/Path.hpp>

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
            return directoryName(_path, _path.allocator());
        }

        String directoryName(const String & _path, Allocator & _alloc)
        {
            return split(_path, _alloc).left;
        }

        String fileName(const String & _path)
        {
            return fileName(_path, _path.allocator());
        }

        String fileName(const String & _path, Allocator & _alloc)
        {
            return split(_path, _alloc).right;
        }

        String extension(const String & _path)
        {
            return extension(_path, _path.allocator());
        }

        String extension(const String & _path, Allocator & _alloc)
        {
            return splitExtension(_path, _alloc).right;
        }

        StringArray segments(const String & _path, char _separator)
        {
            return segments(_path, _path.allocator(), _separator);
        }

        StringArray segments(const String & _path, Allocator & _alloc, char _separator)
        {
            StringArray ret(_alloc);
            ret.reserve(4);
            String::ConstIter it = _path.begin();
            String currentSegment(_alloc);
            while (it != _path.end())
            {
                if (*it == _separator)
                {
                    if (!currentSegment.isEmpty())
                    {
                        ret.append(currentSegment);
                        currentSegment.clear();
                    }
                }
                else currentSegment.append(*it);
                ++it;
            }
            if (!currentSegment.isEmpty())
                ret.append(currentSegment);

            return ret;
        }

        String fromSegments(const StringArray & _segments, bool _bAddLeadingSeparator, bool _bAddTrailingSeparator)
        {
            return fromSegments(_segments, _bAddLeadingSeparator, _bAddTrailingSeparator, _segments.allocator());
        }

        String fromSegments(const StringArray & _segments, bool _bAddLeadingSeparator, bool _bAddTrailingSeparator, Allocator & _alloc)
        {
            String ret(_alloc);
            Size len = 0;
            if (_bAddLeadingSeparator) len++;
            if (_bAddTrailingSeparator) len++;
            for (const String & seg : _segments)
            {
                len += seg.length();
            }
            ret.reserve(len);
            if (_bAddLeadingSeparator) ret.append('/');
            Size off = 0;
            for (Size i = 0; i < _segments.count(); ++i)
            {
                if (i < _segments.count() - 1)
                    ret.append(AppendVariadicFlag(), _segments[i], '/');
                else
                    ret.append(_segments[i]);
            }
            if (_bAddTrailingSeparator) ret.append('/');
            return ret;
        }

        String normalize(const String & _path, bool _bRemoveLeading)
        {
            return normalize(_path, _bRemoveLeading, _path.allocator());
        }

        String normalize(const String & _path, bool _bRemoveLeading, Allocator & _allocator)
        {
            if (_path.isEmpty())
                return String(_allocator);

            bool bHasLeadingSep = _path[0] == '/';
            bool bHasTrailingSep = _path[_path.length() - 1] == '/' && _path.length() > 1;

            StringArray segs = segments(_path, _allocator);
            //to buffer the parsed segments
            StringArray tmp(_allocator);
            tmp.reserve(segs.count());
            StringArray::Iter it = segs.begin();
            for (; it != segs.end(); ++it)
            {
                if (*it == "..")
                {
                    if (!tmp.isEmpty())
                    {
                        //check if the last segment needs to be removed
                        if (tmp.back() == "..")
                            tmp.append(*it);
                        else
                            tmp.removeBack();
                    }
                    //check if the leading segment should be removed
                    else if (!_bRemoveLeading)
                    {
                        tmp.append(*it);
                    }
                }
                //ignore dots
                else if (*it != "." && !(*it).isEmpty())
                {
                    tmp.append(*it);
                }
            }
            return fromSegments(tmp, bHasLeadingSep, bHasTrailingSep, _allocator);
        }

        SplitResult split(const String & _path)
        {
            return split(_path, _path.allocator());
        }

        SplitResult split(const String & _path, Allocator & _alloc)
        {
            Size index = _path.rfindIndex('/');
            if (index != String::InvalidIndex)
            {
                return {_path.sub(0, index, _alloc), _path.sub(index + 1, String::InvalidIndex, _alloc)};
            }

            return {_path, String(_alloc)};
        }

        SplitResult splitExtension(const String & _path)
        {
            return splitExtension(_path, _path.allocator());
        }

        SplitResult splitExtension(const String & _path, Allocator & _alloc)
        {
            Size index = _path.rfindIndex('.');
            if (index != String::InvalidIndex)
            {
                return {_path.sub(0, index, _alloc), _path.sub(index, String::InvalidIndex, _alloc)};
            }
            return {_path, String(_alloc)};
        }

        String join(const String & _a, const String & _b)
        {
            return join(_a, _b, _a.allocator());
        }

        String join(const String & _a, const String & _b, Allocator & _alloc)
        {
            if (isAbsolute(_b))
                return _b;

            if (_a.isEmpty())
                return _b;
            else if (_b.isEmpty())
                return _a;
            else if (_a[_a.length() - 1] == '/')
                return String::concatWithAllocator(_alloc, _a, _b);
            else
                return String::concatWithAllocator(_alloc, _a, '/', _b);
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
