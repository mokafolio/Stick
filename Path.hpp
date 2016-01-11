#ifndef STICK_STICKPATH_HPP
#define STICK_STICKPATH_HPP

#include <Stick/String.hpp>
#include <Stick/DynamicArray.hpp>

namespace stick
{
    //TODO: Should this be somewhere else?
    typedef DynamicArray<String> StringArray;

    namespace path
    {
        /**
         * @brief A pair holding the result of splitting a path string.
         */
        struct SplitResult
        {
            String left;
            String right;
        };

        /**
         * @brief Returns the platform specific path separator.
         */
        String separator();

        /**
         * @brief Returns the directory portion of a path.
         */
        String directoryName(const String & _path);

        String directoryName(const String & _path, Allocator & _allocator);

        /**
         * @brief Returns the file name portion of a path.
         */
        String fileName(const String & _path);

        String fileName(const String & _path, Allocator & _allocator);

        /**
         * @brief Returns the file extension of a path.
         */
        String extension(const String & _path);

        String extension(const String & _path, Allocator & _allocator);

        /**
         * @brief Returns all the segments/portions of a path as individual strings.
         */
        StringArray segments(const String & _path);

        StringArray segments(const String & _path, Allocator & _allocator);

        /**
         * @brief Creates a path string from individual path segments.
         */
        String fromSegments(const StringArray & _segments, bool _bAddLeadingSeparator = false, bool _bAddTrailingSeparator = false);

        String fromSegments(const StringArray & _segments, bool _bAddLeadingSeparator, bool _bAddTrailingSeparator, Allocator & _alloc);

        /**
         * @brief Normalize a pathname by collapsing redundant separators and up-level references so that A//B, A/B/, A/./B and A/foo/../B all become A/B
         */
        String normalize(const String & _path, bool _bRemoveLeading = true);

        String normalize(const String & _path, bool _bRemoveLeading, Allocator & _alloc);

        /**
         * @brief Split a path into two parts.
         * SplitResult.first is everything leading up to the last path segment.
         * SplitResult.second is the last path segment.
         * @see SplitResult
         */
        SplitResult split(const String & _path);

        SplitResult split(const String & _path, Allocator & _alloc);

        /**
         * @brief Splits a path from its file extension.
         * SplitResult.first will be the path.
         * SplitResult.second will hold the extension.
         * @see SplitResult
         */
        SplitResult splitExtension(const String & _path);

        SplitResult splitExtension(const String & _path, Allocator & _allocator);

        /**
         * @brief Joins two paths.
         *
         * If the second path is absolute, join will return it. If it is relative,
         * both paths are joined. The function makes sure that the resulting path
         * won't have a double separator between the two joined paths.
         */
        String join(const String & _a, const String & _b);

        String join(const String & _a, const String & _b, Allocator & _allocator);

        /**
         * @brief Returns true if a path is relative.
         */
        bool isRelative(const String & _path);

        /**
         * @brief Returns true if a path is absolute.
         */
        bool isAbsolute(const String & _path);
    }
}

#endif //STICK_STICKPATH_HPP
