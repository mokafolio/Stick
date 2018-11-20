#include <Stick/FileSystem.hpp>
#include <Stick/Path.hpp>

#ifdef STICK_PLATFORM_POSIX
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#endif // STICK_PLATFORM_POSIX

namespace stick
{
namespace fs
{
FileStatus::FileStatus() : m_bIsValid(false)
{
}

FileStatus::FileStatus(const String & _path) : m_bIsValid(true)
{
#ifdef STICK_PLATFORM_POSIX
    // check if this is a symlink
    if (lstat(_path.cString(), &m_nativeStatus) == 0)
    {
        if ((m_nativeStatus.st_mode & S_IFMT) == S_IFLNK)
            return;
    }
    // else get the normal file status, this does no error checking since it must not throw
    // as FileStatus is also used by the non throwing FileSystem functions
    if (stat(_path.cString(), &m_nativeStatus) == -1)
        m_bIsValid = false;
#endif // STICK_PLATFORM_POSIX
}

FileStatus::FileStatus(const FileStatus & _other) :
    m_bIsValid(_other.m_bIsValid),
    m_nativeStatus(_other.m_nativeStatus)
{
}

FileStatus & FileStatus::operator=(const FileStatus & _other)
{
    m_nativeStatus = _other.m_nativeStatus;
    m_bIsValid = _other.m_bIsValid;
    return *this;
}

FileType FileStatus::type() const
{
#ifdef STICK_PLATFORM_POSIX
    if ((m_nativeStatus.st_mode & S_IFMT) == S_IFDIR)
        return FileType::Directory;
    else if ((m_nativeStatus.st_mode & S_IFMT) == S_IFREG)
        return FileType::RegularFile;
    else if ((m_nativeStatus.st_mode & S_IFMT) == S_IFLNK)
        return FileType::SymbolicLink;
    else
        return FileType::Other;
#endif // STICK_PLATFORM_POSIX
}

PermissionFlags FileStatus::permissions() const
{
    UInt32 ret = 0;
#ifdef STICK_PLATFORM_POSIX
    if (m_nativeStatus.st_mode & S_IRUSR)
        ret |= static_cast<UInt32>(Permission::OwnerRead);
    if (m_nativeStatus.st_mode & S_IWUSR)
        ret |= static_cast<UInt32>(Permission::OwnerWrite);
    if (m_nativeStatus.st_mode & S_IXUSR)
        ret |= static_cast<UInt32>(Permission::OwnerExe);
    if (m_nativeStatus.st_mode & S_IXUSR)
        ret |= static_cast<UInt32>(Permission::OwnerExe);

    if (m_nativeStatus.st_mode & S_IRGRP)
        ret |= static_cast<UInt32>(Permission::GroupRead);
    if (m_nativeStatus.st_mode & S_IWGRP)
        ret |= static_cast<UInt32>(Permission::GroupWrite);
    if (m_nativeStatus.st_mode & S_IXGRP)
        ret |= static_cast<UInt32>(Permission::GroupExe);

    if (m_nativeStatus.st_mode & S_IROTH)
        ret |= static_cast<UInt32>(Permission::OthersRead);
    if (m_nativeStatus.st_mode & S_IWOTH)
        ret |= static_cast<UInt32>(Permission::OthersWrite);
    if (m_nativeStatus.st_mode & S_IXOTH)
        ret |= static_cast<UInt32>(Permission::OthersExe);

    if (m_nativeStatus.st_mode & S_ISUID)
        ret |= static_cast<UInt32>(Permission::SetUserIdOnExe);
    if (m_nativeStatus.st_mode & S_ISGID)
        ret |= static_cast<UInt32>(Permission::SetGroupIdOnExe);
    if (m_nativeStatus.st_mode & S_ISVTX)
        ret |= static_cast<UInt32>(Permission::StickyBit);
#endif // STICK_PLATFORM_POSIX

    return static_cast<PermissionFlags>(ret);
}

bool FileStatus::isValid() const
{
    return m_bIsValid;
}

DirectoryEntry::DirectoryEntry()
{
}

DirectoryEntry::DirectoryEntry(const String & _path) : m_path(_path), m_status(_path)
{
}

const String & DirectoryEntry::path() const
{
    return m_path;
}

const FileStatus & DirectoryEntry::status() const
{
    return m_status;
}

bool DirectoryEntry::operator==(const DirectoryEntry & _other) const
{
    return m_path == _other.m_path;
}

bool DirectoryEntry::operator!=(const DirectoryEntry & _other) const
{
    return m_path != _other.m_path;
}

bool DirectoryEntry::operator<(const DirectoryEntry & _other) const
{
    return m_path < _other.m_path;
}

bool DirectoryEntry::operator<=(const DirectoryEntry & _other) const
{
    return m_path <= _other.m_path;
}

bool DirectoryEntry::operator>(const DirectoryEntry & _other) const
{
    return m_path > _other.m_path;
}

bool DirectoryEntry::operator>=(const DirectoryEntry & _other) const
{
    return m_path >= _other.m_path;
}

DirectoryIterator DirectoryIterator::End;

DirectoryIterator::DirectoryIterator() : m_dir(nullptr)
{
}

DirectoryIterator::DirectoryIterator(const String & _path) : m_dir(nullptr), m_path(_path)
{
    m_dir = opendir(_path.cString());
    if (!m_dir)
        m_error = Error(ec::SystemErrorCode(errno), "", STICK_FILE, STICK_LINE);
    increment();
}

DirectoryIterator::DirectoryIterator(const DirectoryIterator & _other) :
    m_dir(_other.m_dir),
    m_currentEntry(_other.m_currentEntry),
    m_path(_other.m_path),
    m_error(_other.m_error)
{
}

DirectoryIterator::~DirectoryIterator()
{
    if (m_dir)
        closedir(m_dir);
}

DirectoryIterator & DirectoryIterator::increment()
{
    STICK_ASSERT(m_dir);
    errno = 0;
    struct dirent * de = nullptr;
    String tmp;
    de = readdir(m_dir);
    if (de)
    {
        tmp = String(de->d_name);
        // skip these
        if (tmp == "." || tmp == "..")
        {
            return increment();
        }

        m_currentEntry = DirectoryEntry(path::join(m_path, tmp));
    }
    else
    {
        // end is reached or error
        m_currentEntry = DirectoryEntry();
        m_path.clear();
        if (errno != 0)
            m_error = Error(ec::SystemErrorCode(errno), "", STICK_FILE, STICK_LINE);
    }

    return *this;
}

bool DirectoryIterator::operator==(const DirectoryIterator & _other) const
{
    return m_path == _other.m_path && m_currentEntry == _other.m_currentEntry;
}

bool DirectoryIterator::operator!=(const DirectoryIterator & _other) const
{
    return m_path != _other.m_path || m_currentEntry != _other.m_currentEntry;
}

const DirectoryEntry & DirectoryIterator::operator*() const
{
    return currentDirectoryEntry();
}

const DirectoryEntry * DirectoryIterator::operator->() const
{
    return &currentDirectoryEntry();
}

DirectoryIterator & DirectoryIterator::operator++()
{
    return increment();
}

const DirectoryEntry & DirectoryIterator::currentDirectoryEntry() const
{
    return m_currentEntry;
}

const String & DirectoryIterator::path() const
{
    return m_path;
}

Error DirectoryIterator::error() const
{
    return m_error;
}

// recursive directory iterator

RecursiveDirectoryIterator RecursiveDirectoryIterator::End;

RecursiveDirectoryIterator::RecursiveDirectoryIterator() :
    m_node(nullptr),
    m_bGotSubDirectory(false)
{
}

RecursiveDirectoryIterator::RecursiveDirectoryIterator(const String & _path) :
    m_node(defaultAllocator().create<Node>(_path, nullptr)),
    m_bGotSubDirectory(false)
{
    // check if the first entry is another dir
    if (m_node->m_it != DirectoryIterator::End)
    {
        if (isDirectory(m_node->m_it->path()))
        {
            m_bGotSubDirectory = true;
        }
    }
}

RecursiveDirectoryIterator::~RecursiveDirectoryIterator()
{
    if (m_node)
        defaultAllocator().destroy(m_node);
}

RecursiveDirectoryIterator::RecursiveDirectoryIterator(const RecursiveDirectoryIterator & _other)
{
    m_node = _other.m_node;
}

bool RecursiveDirectoryIterator::operator==(const RecursiveDirectoryIterator & _other)
{
    return m_node == _other.m_node;
}

bool RecursiveDirectoryIterator::operator!=(const RecursiveDirectoryIterator & _other)
{
    return m_node != _other.m_node;
}

const DirectoryEntry & RecursiveDirectoryIterator::operator*() const
{
    return *m_node->m_it;
}

const DirectoryEntry * RecursiveDirectoryIterator::operator->() const
{
    return &(*m_node->m_it);
}

RecursiveDirectoryIterator & RecursiveDirectoryIterator::operator++()
{
    if (m_node)
    {
        // get the first entry of the new subdirectory
        if (m_bGotSubDirectory)
        {
            Node * newNode = defaultAllocator().create<Node>(m_node->m_it->path(), m_node);
            m_node = newNode;
            m_bGotSubDirectory = false;

            // if the new directory is empty, we iterate again, to go back to it's parent
            if (m_node->m_it == DirectoryIterator::End)
                this->operator++();
        }
        else
        {
            // iterate the current directory
            ++(m_node->m_it);

            // check if we reached the end, return to parent if there is one
            if (m_node->m_it == DirectoryIterator::End)
            {
                if (m_node->m_parent)
                {
                    Node * old = m_node;
                    m_node = m_node->m_parent;
                    defaultAllocator().destroy(old);
                    // we allready visited the it position of parent, so we advance again
                    this->operator++();
                }
                else
                {
                    // we reached the end!
                    defaultAllocator().destroy(m_node);
                    m_node = nullptr;
                }
            }
            // check if this is another directory, if so, set flag so a new node is created
            // on the next iteration.
            else if (isDirectory(m_node->m_it->path()))
            {
                m_bGotSubDirectory = true;
            }
        }
    }

    return *this;
}

Error createDirectories(const String & _path)
{
    auto segs = path::segments(_path);
    String tmp;
    for (auto & seg : segs)
    {
        tmp = path::join(tmp, seg);
        auto err = createDirectory(tmp);
        if (err)
            return err;
    }
    return Error();
}

Error createDirectory(const String & _path)
{
    int result = mkdir(_path.cString(), 0777);
    if (result == -1)
        Error(ec::SystemErrorCode(errno), "Could not create directory.", STICK_FILE, STICK_LINE);
    return Error();
}

Error createSymbolicLink(const String & _path1, const String & _path2)
{
    int result = symlink(_path1.cString(), _path2.cString());
    if (result == -1)
        Error(
            ec::SystemErrorCode(errno), "Could not create symbolic link.", STICK_FILE, STICK_LINE);
    return Error();
}

Error copy(const String & _from, const String & _to)
{
    FileStatus st(_from);
    if (st.type() == FileType::RegularFile)
        return copyFile(_from, _to);
    else if (st.type() == FileType::SymbolicLink)
        return copySymbolicLink(_from, _to);
    else if (st.type() == FileType::Directory)
        return copyDirectory(_from, _to);
    return Error(ec::InvalidOperation, "Could not determine file type.", STICK_FILE, STICK_LINE);
}

Error copyAll(const String & _from, const String & _to)
{
    FileStatus st(_from);
    if (st.type() == FileType::RegularFile || st.type() == FileType::SymbolicLink)
    {
        return copy(_from, _to);
    }
    else if (st.type() == FileType::Directory)
    {
        Error err = copyDirectory(_from, _to);
        if (err)
            return err;
        DirectoryIterator it(_from);
        if (it.error())
            return it.error();

        for (; it != DirectoryIterator::End; it.increment())
        {
            if (it.error())
                return it.error();
            String name = path::fileName(it.path());
            String to = path::join(_to, name);
            err = copyAll(it.path(), to);
            if (err)
                return err;
        }
    }
    return Error();
}

Error copyDirectory(const String & _from, const String & _to)
{
    FileStatus st(_from);
    if (st.isValid() && st.type() == FileType::Directory)
        return createDirectory(_to);
    else
        return Error(ec::InvalidOperation, "Not a directory.", STICK_FILE, STICK_LINE);
}

Error copySymbolicLink(const String & _from, const String & _to)
{
    auto tmp = readSymbolicLink(_from);
    if (tmp)
        return createSymbolicLink(tmp.get(), _to);
    return tmp.error();
}

Error copyFile(const String & _from, const String & _to)
{
    Error ret;
    int fdFrom, fdTo;
    char buffer[4096];
    Size numBytesRead;

    fdFrom = open(_from.cString(), O_RDONLY);
    if (fdFrom == -1)
    {
        ret = Error(ec::SystemErrorCode(errno), "", STICK_FILE, STICK_LINE);
    }

    fdTo = open(_to.cString(), O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fdTo == -1)
    {
        ret = Error(ec::SystemErrorCode(errno), "", STICK_FILE, STICK_LINE);
        close(fdFrom);
    }

    bool bBreak = false;
    while (numBytesRead = read(fdFrom, buffer, sizeof(buffer)), numBytesRead > 0 && !bBreak)
    {
        char * outPtr = buffer;
        Int64 numBytesWritten;
        do
        {
            numBytesWritten = write(fdTo, outPtr, numBytesRead);

            if (numBytesWritten >= 0)
            {
                numBytesRead -= numBytesWritten;
                outPtr += numBytesWritten;
            }
            else if (errno != EINTR)
            {
                bBreak = true;
                break;
            }
        } while (numBytesRead > 0);
    }

    // error, retrieve it
    if (bBreak)
        ret = Error(ec::SystemErrorCode(errno), "", STICK_FILE, STICK_LINE);

    close(fdFrom);
    close(fdTo);

    return ret;
}

bool exists(const String & _path)
{
    FileStatus tmp(_path);
    return tmp.isValid();
}

Result<Size> fileSize(const String & _path)
{
    struct stat st;
    int result = stat(_path.cString(), &st);
    if (result == -1)
        return Error(ec::SystemErrorCode(errno), "", STICK_FILE, STICK_LINE);
    return st.st_size;
}

bool isDirectory(const FileStatus & _status)
{
    return _status.type() == FileType::Directory;
}

bool isDirectory(const String & _path)
{
    FileStatus tmp(_path);
    return tmp.type() == FileType::Directory;
}

bool isFile(const FileStatus & _status)
{
    return _status.type() == FileType::RegularFile;
}

bool isFile(const String & _path)
{
    FileStatus tmp(_path);
    return tmp.type() == FileType::RegularFile;
}

bool isSymbolicLink(const FileStatus & _status)
{
    return _status.type() == FileType::SymbolicLink;
}

bool isSymbolicLink(const String & _path)
{
    FileStatus tmp(_path);
    return tmp.type() == FileType::SymbolicLink;
}

Result<DirectoryEntryArray> listDirectory(const String & _path, Allocator & _alloc)
{
    DirectoryEntryArray ret;
    DirectoryIterator it(_path);
    if (it.error())
        return it.error();
    ret.reserve(32);
    for (; it != DirectoryIterator::End; it.increment())
    {
        if (it.error())
            return it.error();
        ret.append(*it);
    }
    return ret;
}

PermissionFlags permissions(const String & _path)
{
    FileStatus tmp(_path);
    return tmp.permissions();
}

Result<String> readSymbolicLink(const String & _path)
{
    // since there is no portable way of knowing the path length needed in advance, we
    // loop and double the array length on each iteration until an error occurs, or the buffer
    // is big enough to hold the path.
    char p[256];
    Size actualLen = readlink(_path.cString(), p, 256);
    if (actualLen == (Size)-1)
    {
        return Error(ec::SystemErrorCode(errno), "", STICK_FILE, STICK_LINE);
    }
    else
    {
        // according to http://pubs.opengroup.org/onlinepubs/007904875/functions/readlink.html
        // readlink does not guarantee null-termination, so we manually make sure that the string is
        // terminated.
        p[actualLen] = '\0';
        // TODO: Get rid of this extra copy?
        return String(p);
    }
}

Error remove(const String & _path)
{
    int result;
    FileStatus st(_path);
    if (st.type() == FileType::SymbolicLink || st.type() == FileType::RegularFile)
        result = unlink(_path.cString());
    else
        result = rmdir(_path.cString());

    if (result == -1)
        return Error(ec::SystemErrorCode(errno), "", STICK_FILE, STICK_LINE);

    return Error();
}

Error removeAll(const String & _path)
{
    FileStatus st(_path);
    if (!st.isValid())
        return Error(ec::InvalidOperation, "The path is invalid.", STICK_FILE, STICK_LINE);

    if (st.type() == FileType::RegularFile || st.type() == FileType::SymbolicLink)
    {
        return remove(_path);
    }
    else if (st.type() == FileType::Directory)
    {
        DirectoryIterator it(_path);
        if (it.error())
            return it.error();
        for (; it != DirectoryIterator::End; it.increment())
        {
            if (it.error())
                return it.error();
            Error err = removeAll(it->path());
            if (err)
                return err;
        }
        return remove(_path);
    }

    return Error();
}

Error setWorkingDirectory(const String & _path)
{
    int result = chdir(_path.cString());
    if (result == -1)
        return Error(ec::SystemErrorCode(errno), "", STICK_FILE, STICK_LINE);
    return Error();
}

Result<String> workingDirectory()
{
    // since there is no portable way of knowing the path length needed in advance, we
    // loop and double the array length on each iteration until an error occurs, or the buffer
    // is big enough to hold the path.

    for (Size len = 128;; len *= 2)
    {
        char p[len];
        char * result = getcwd(p, len);
        if (!result)
        {
            if (errno == ERANGE)
            {
                continue;
            }
            else
            {
                return Error(ec::SystemErrorCode(errno), "", STICK_FILE, STICK_LINE);
            }
        }
        else
        {
            // unfortunatelly we need to copy to the String here since pre C++11 does not
            // guarantee that it std::string is continous in memory.
            return String(p);
        }
    }
}
} // namespace fs
} // namespace stick
