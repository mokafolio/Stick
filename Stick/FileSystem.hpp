#ifndef STICK_STICKFILESYSTEM_HPP
#define STICK_STICKFILESYSTEM_HPP

#include <Stick/String.hpp>
#include <Stick/Result.hpp>
#include <Stick/DynamicArray.hpp>
#include <iterator>

#ifdef STICK_PLATFORM_POSIX
#include <dirent.h>
#include <sys/stat.h> //can't forward declare :(
#endif //STICK_PLATFORM_POSIX

namespace stick
{
    namespace fs
    {
        /**
         * @brief The type of a file system item.
         */
        enum class STICK_API FileType
        {
            /**
             * @brief A regular file.
             */
            RegularFile,
            /**
             * @brief A symbolic link, pointing to another file system item.
             */
            SymbolicLink,
            /**
             * @brief A directory.
             */
            Directory,
            /**
             * @brief An unknown, platform specific item.
             */
            Other
        };

        /**
         * @brief Unix style file permissions.
         *
         * This might have to change or be abstracted on non-unix platforms.
         */
        enum class STICK_API Permission
        {
            None = 0,
            OwnerRead = 0400,
            OwnerWrite = 0200,
            OwnerExe = 0100,
            OwnerAll = OwnerRead | OwnerWrite | OwnerExe,
            GroupRead = 040,
            GroupWrite = 020,
            GroupExe = 010,
            GroupAll = GroupRead | GroupWrite | GroupExe,
            OthersRead = 04,
            OthersWrite = 02,
            OthersExe = 01,
            OthersAll = OthersRead | OthersWrite | OthersExe,
            AllAll = OwnerAll | GroupAll | OthersAll,
            SetUserIdOnExe = 04000,
            SetGroupIdOnExe = 02000,
            StickyBit = 01000
        };

        /**
         * @brief An integer holding permission flags.
         */
        typedef UInt32 PermissionFlags;

        namespace detail
        {
#ifdef STICK_PLATFORM_POSIX
            typedef struct stat NativeFileStatusType;
#endif //STICK_PLATFORM_POSIX
        }

        /**
        * @brief Describes the properties of a file.
        */
        class STICK_API FileStatus
        {
        public:

            /**
             * @brief Default Constructor.
             */
            FileStatus();

            /**
             * @brief Constructs a file status for the provided path.
             */
            FileStatus(const String & _path);

            /**
             * @brief Constructs a FileStatus from another status.
             */
            FileStatus(const FileStatus & _other);

            /**
             * @brief Assigns an existing file status to this.
             */
            FileStatus & operator = (const FileStatus & _other);

            /**
             * @brief Returns the type of this file.
             */
            FileType type() const;

            /**
             * @brief Returns the permissions associated with this file.
             */
            PermissionFlags permissions() const;

            /**
             * @brief Returns true if this is a valid file status.
             */
            bool isValid() const;

        private:

            bool m_bIsValid;
            PermissionFlags m_permissions;
            detail::NativeFileStatusType m_nativeStatus;
        };

        /**
         * @brief Describes an element in a directory.
         */
        class STICK_API DirectoryEntry
        {
        public:

            /**
             * @brief Default Constructor.
             */
            DirectoryEntry();

            /**
             * @brief Creates a directory entry with with provided path.
             */
            explicit DirectoryEntry(const String & _path);

            /**
             * @brief Returns the path of the entry.
             */
            const String & path() const;

            /**
             * @brief Returns the file status of the entry.
             */
            const FileStatus & status() const;

            /**
             * @brief Returns true if this entry has the same path as the other one.
             */
            bool operator == (const DirectoryEntry & _other) const;

            /**
             * @brief Returns true if this entry has not the same path as the other one.
             */
            bool operator != (const DirectoryEntry & _other) const;

            /**
             * @brief To sort by path.
             */
            bool operator < (const DirectoryEntry & _other) const;

            /**
             * @brief To sort by path.
             */
            bool operator <= (const DirectoryEntry & _other) const;

            /**
             * @brief To sort by path.
             */
            bool operator > (const DirectoryEntry & _other) const;

            /**
             * @brief To sort by path.
             */
            bool operator >= (const DirectoryEntry & _other) const;

        private:

            String m_path;
            FileStatus m_status;
        };

        /**
         * @brief An array of directory entries.
         */
        typedef DynamicArray<DirectoryEntry> DirectoryEntryArray;

        /**
         * @brief A directory iterator allows you to iterate over the files in one directory.
         */
        class STICK_API DirectoryIterator :
            public std::iterator<std::input_iterator_tag, DirectoryEntry>
        {
        public:

            /**
             * @brief Symbolic end iterator.
             */
            static DirectoryIterator End;

            /**
             * @brief Default Constructor.
             */
            DirectoryIterator();

            /**
             * @brief Destructor.
             */
            ~DirectoryIterator();

            /**
             * @brief Constructs a directory iterator for the directory at the provided path.
             * @param _path The path of the directory.
             */
            explicit DirectoryIterator(const String & _path);

            /**
             * @brief Constructs a directory iterator from an existing iterator.
             */
            DirectoryIterator(const DirectoryIterator & _other);

            /**
             * @brief Returns true if this iterator and the provided one point to the same path.
             */
            bool operator == (const DirectoryIterator & _other) const;

            /**
             * @brief Returns true if this iterator and the provided one point to a different path.
             */
            bool operator != (const DirectoryIterator & _other) const;

            /**
             * @brief Dereferences the iterator.
             * @return The current directory entry.
             */
            const DirectoryEntry & operator * () const;

            /**
             * @brief Dereferences the iterator.
             * @return The current directory entry.
             */
            const DirectoryEntry * operator -> () const;

            /**
             * @brief Increments the iterator.
             */
            DirectoryIterator & operator ++ ();

            /**
             * @brief Increments the iterator.
             * @param _error Stores the error if the incerementation failed.
             */
            DirectoryIterator & increment();

            Error error() const;

            const String & path() const;

            const DirectoryEntry & currentDirectoryEntry() const;

        private:

            DIR * m_dir;
            DirectoryEntry m_currentEntry;
            String m_path;
            Error m_error;
        };

        /**
         * @brief An iterator to recursively iterate over a directory structure.
         */
        class STICK_API RecursiveDirectoryIterator :
            public std::iterator<std::input_iterator_tag, DirectoryEntry>
        {
        public:

            /**
             * @brief Symbolic end iterator.
             */
            //end is reached if m_current == 0 and m_parent == 0
            static RecursiveDirectoryIterator End;

            /**
             * @brief Default Constructor.
             */
            RecursiveDirectoryIterator();

            /**
             * @brief Constructs a recursive iterator with the provided path.
             */
            explicit RecursiveDirectoryIterator(const String & _path);

            /**
             * @brief Destructor.
             */
            ~RecursiveDirectoryIterator();

            RecursiveDirectoryIterator(const RecursiveDirectoryIterator & _other);


            bool operator == (const RecursiveDirectoryIterator & _other);

            bool operator != (const RecursiveDirectoryIterator & _other);


            const DirectoryEntry & operator * () const;

            const DirectoryEntry * operator -> () const;

            RecursiveDirectoryIterator & operator ++ ();

        private:


            struct Node
            {
                Node(const String & _path, Node * _parent):
                    m_it(_path),
                    m_parent(_parent)
                {

                }

                Node * m_parent;
                DirectoryIterator m_it;
            };

            Node * m_node;
            bool m_bGotSubDirectory; //flag that indicates that a new node should be created on the next iteration
        };

        /**
         * @brief Recursively creates the requested directory structure.
         */
        STICK_API Error createDirectories(const String & _path);

        /**
         * @brief Creates a directory at the specified path.
         */
        STICK_API Error createDirectory(const String & _path);

        /**
         * @brief Creates a symbolic link.
         * @param _path1 The path to link to.
         * @param _path2 The name of the symbolic link.
         */
        STICK_API Error createSymbolicLink(const String & _path1, const String & _path2);

        /**
         * @brief Copy a file/directory/symbolic link.
         * @param _from The path to the item to copy.
         * @param _to The path to copy the item to.
         */
        STICK_API Error copy(const String & _from, const String & _to);

        /**
         * @brief Recursively copy a directory structure.
         * @param _from The path to the item to copy.
         * @param _to The path to copy the item to.
         */
        STICK_API Error copyAll(const String & _from, const String & _to);

        /**
         * @brief Copy a directory.
         * @param _from The path to the directory to copy.
         * @param _to The path to copy the directory to.
         */
        STICK_API Error copyDirectory(const String & _from, const String & _to);

        /**
         * @brief Copy a file.
         * @param _from The path to the file to copy.
         * @param _to The path to copy the file to.
         */
        STICK_API Error copyFile(const String & _from, const String & _to);

        /**
         * @brief Copy a symbolic link.
         * @param _from The path to the link to copy.
         * @param _to The path to copy the link to.
         * @throws SystemErrorException
         */
        STICK_API Error copySymbolicLink(const String & _from, const String & _to);

        /**
         * @brief Check if a path exists.
         * @param _path The path to check.
         * @return true if the path exists.
         */
        STICK_API bool exists(const String & _path);

        /**
        * @brief Get the byte size of a file.
        * @param _path The path of the file to get the size of.
        * @return A Result<Size> holding an error or the size.
        */
        STICK_API Result<Size> fileSize(const String & _path);

        /**
            * @brief Returns true if the FileStatus is associated with a directory.
            */
        STICK_API bool isDirectory(const FileStatus & _status);

        /**
         * @brief Returns true if the provided path points to a directory.
         */
        STICK_API bool isDirectory(const String & _path);

        /**
         * @brief Returns true if the FileStatus is associated with a file.
         */
        STICK_API bool isFile(const FileStatus & _status);

        /**
         * @brief Returns true if the provided path points to a file.
         */
        STICK_API bool isFile(const String & _path);

        /**
         * @brief Returns true if the FileStatus is associated with a symbolic link.
         */
        STICK_API bool isSymbolicLink(const FileStatus & _status);

        /**
         * @brief Returns true if the provided path points to a symbolic link.
         */
        STICK_API bool isSymbolicLink(const String & _path);

        /**
         * @brief Returns an array of all the entries in the directory at the provided path.
         * @param _path The path of the directory.
         */
        STICK_API Result<DirectoryEntryArray> listDirectory(const String & _path, Allocator & _alloc = defaultAllocator());

        /**
         * @brief Returns the permission of an item located at the provided path.
         */
        STICK_API PermissionFlags permissions(const String & _path);

        /**
         * @brief Returns the path that a symbolic link points to.
         * @param _path The path of the symbolic link.
         */
        STICK_API Result<String> readSymbolicLink(const String & _path);

        /**
         * @brief Removes a file system item.
         * @param _path The path to the item to remove.
         */
        STICK_API Error remove(const String & _path);

        /**
         * @brief Recursively removes a directory structure.
         *
         * If the path does not point to a directory, but a file or symbolic link, that item
         * will be removed instead.
         * @param _path The path to remove.
         */
        STICK_API Error removeAll(const String & _path);

        /**
         * @brief Sets the current working directory.
         * @param _path The path to set the working directory to.
         * @throws SystemErrorException
         */
        STICK_API Error setWorkingDirectory(const String & _path);

        /**
         * @brief Returns the current working directory.
         * @throws SystemErrorException
         */
        STICK_API Result<String> workingDirectory();
    }
}

#endif //STICK_STICKFILESYSTEM_HPP
