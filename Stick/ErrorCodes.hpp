#ifndef STICK_ERRORCODES_HPP
#define STICK_ERRORCODES_HPP

#include <errno.h>

namespace stick
{
    namespace ec
    {
        enum MiscErrorCode
        {
            NoError = 0,
            InvalidOperation,
            ParseFailed,
            ComposeFailed,
            BadURI,
            Unsupported
        };

        /**
         * @brief System errors.
         *
         * based on http://pubs.opengroup.org/onlinepubs/009604599/basedefs/errno.h.html
         */
        enum SystemErrorCode
        {
            AddressFamilyNotSupported = EAFNOSUPPORT,
            AddressInUse = EADDRINUSE,
            AddressNotAvailable = EADDRNOTAVAIL,
            AlreadyConnected = EISCONN,
            ArgumentListTooLong = E2BIG,
            ArgumentOutOfDomain = EDOM,
            BadAddress = EFAULT,
            BadFileDescriptor = EBADF,
            BadMessage = EBADMSG,
            BrokenPipe = EPIPE,
            ConnectionAborted = ECONNABORTED,
            ConnectionAlreadyInProgress = EALREADY,
            ConnectionRefused = ECONNREFUSED,
            ConnectionReset = ECONNRESET,
            CrossDeviceLink = EXDEV,
            DestinationAddressRequired = EDESTADDRREQ,
            DeviceOrResourceBusy = EBUSY,
            DirectoryNotEmpty = ENOTEMPTY,
            ExecutableFormatError = ENOEXEC,
            FileExists = EEXIST,
            FilenameTooLong = ENAMETOOLONG,
            FileTooLarge = EFBIG,
            FunctionNotSupported = ENOSYS,
            HostIsUnreachable = EHOSTUNREACH,
            IdentifierRemoved = EIDRM,
            IllegalByteSequence = EILSEQ,
            InappropriateIoControlOperation = ENOTTY,
            Interrupted = EINTR,
            InvalidArgument = EINVAL,
            InvalidSeek = ESPIPE,
            IOError = EIO,
            IsDirectory = EISDIR,
            MessageTooLarge = EMSGSIZE,
            NetworkDown = ENETDOWN,
            NetworkReset = ENETRESET,
            NetworkUnreachable = ENETUNREACH,
            NoBufferSpace = ENOBUFS,
            NoChildProcesses = ECHILD,
            NoLink = ENOLINK,
            NoLocksAvaiable = ENOLCK,
            NoMemory = ENOMEM,
            NoMessage = ENOMSG,
            NoMessageAvailable = ENODATA,
            NoSpaceOnDevice = ENOSPC,
            NoStreamResources = ENOSR,
            NoSuchDevice = ENODEV,
            NoSuchDeviceOrAddress = ENXIO,
            NoSuchFileOrDirectory = ENOENT,
            NoSuchProcess = ESRCH,
            NotADirectory = ENOTDIR,
            NotASocket = ENOTSOCK,
            NotAStream = ENOSTR,
            NotConnected = ENOTCONN,
            NotSupported = ENOTSUP,
            OperationCanceled = ECANCELED,
            OperationInProgress = EINPROGRESS,
            OperationNotPermitted = EPERM,
            OperationNotSupported = EOPNOTSUPP,
            PermissionDenied = EACCES,
            ProtocolError = EPROTO,
            ProtocolNotAvailable = ENOPROTOOPT,
            ProtocolNotSupported = EPROTONOSUPPORT,
            ReadOnlyFileSystem = EROFS,
            ResourceDeadlockWouldOccur = EDEADLK,
            ResourceUnavailableTryAgain = EAGAIN,
            ResultOutOfRange = ERANGE,
            StreamTimeout = ETIME,
            TextFileBusy = ETXTBSY,
            TimedOut = ETIMEDOUT,
            TooManyFilesOpen = ENFILE,
            TooManyLinks = EMLINK,
            TooManyOpenFiles = EMFILE,
            TooManySymbolicLinkLevels = ELOOP,
            ValueTooLarge = EOVERFLOW,
            WouldBlock = EWOULDBLOCK,
            WrongProtocolType = EPROTOTYPE
        };
    }
}

#endif //STICK_ERRORCODES_HPP
