#include <Stick/StickPath.hpp>

#include <iostream>

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
            return split(_path).left;
        }

        String fileName(const String & _path)
        {
            return split(_path).right;
        }

        String extension(const String & _path)
        {
            return splitExtension(_path).right;
        }

        StringArray segments(const String & _path)
        {
            StringArray ret;
            ret.reserve(4);
            String::ConstIter it = _path.begin();
            String currentSegment;
            while (it != _path.end())
            {
                std::cout<<*it<<std::endl;
                if (*it == '/')
                {
                    if (!currentSegment.isEmpty())
                    {
                        std::cout<<"CS: "<<currentSegment.cString()<<std::endl;
                        ret.append(currentSegment);
                        std::cout<<"POST APPEND: "<<currentSegment.cString()<<std::endl;
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
            String ret;
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
            for (Size i=0; i < _segments.elementCount(); ++i)
            {   
                if(i < _segments.elementCount() - 1)
                    ret.append(_segments[i], '/');
                else
                    ret.append(_segments[i]);
            }
            if (_bAddTrailingSeparator) ret.append('/');
            return ret;
        }

        String normalize(const String & _path, bool _bRemoveLeading)
        {
            if (_path.isEmpty())
                return String();

            bool bHasLeadingSep = _path[0] == '/';
            bool bHasTrailingSep = _path[_path.length() - 1] == '/';

            StringArray segs = segments(_path);
            //to buffer the parsed segments
            StringArray tmp;
            tmp.reserve(segs.elementCount());
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
            return fromSegments(tmp, bHasLeadingSep, bHasTrailingSep);
        }

        SplitResult split(const String & _path)
        {
            Size index = _path.rfindIndex('/');
            if (index != String::InvalidIndex)
            {
                return {_path.sub(0, index), _path.sub(index + 1)};
            }

            return {_path, String()};
        }

        SplitResult splitExtension(const String & _path)
        {
            Size index = _path.rfindIndex('.');
            if (index != String::InvalidIndex)
            {
                return {_path.sub(0, index), _path.sub(index)};
            }
            return {_path, String()};
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
