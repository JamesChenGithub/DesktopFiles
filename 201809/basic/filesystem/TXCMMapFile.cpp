//
// Created by alderzhang on 2017/5/19.
//

#include "TXCMMapFile.h"

#if defined(_WIN32)
# define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
# include <windows.h>

#if UWP

DWORD SetFilePointer(
    _In_ HANDLE hFile,
    _In_ LONG lDistanceToMove,
    _Inout_opt_ PLONG lpDistanceToMoveHigh,
    _In_ DWORD dwMoveMethod
    );


HANDLE
CreateFileW(
    _In_ LPCWSTR lpFileName,
    _In_ DWORD dwDesiredAccess,
    _In_ DWORD dwShareMode,
    _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    _In_ DWORD dwCreationDisposition,
    _In_ DWORD dwFlagsAndAttributes,//
    _In_opt_ HANDLE hTemplateFile
    );




DWORD GetFileSize(
    _In_      HANDLE  hFile,
    _Out_opt_ LPDWORD lpFileSizeHigh
    );


HANDLE
WINAPI
CreateFileA(
    _In_ LPCSTR lpFileName,
    _In_ DWORD dwDesiredAccess,
    _In_ DWORD dwShareMode,
    _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    _In_ DWORD dwCreationDisposition,
    _In_ DWORD dwFlagsAndAttributes,
    _In_opt_ HANDLE hTemplateFile
    );


HANDLE
CreateFileMappingA(
    _In_     HANDLE hFile,
    _In_opt_ LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
    _In_     DWORD flProtect,
    _In_     DWORD dwMaximumSizeHigh,
    _In_     DWORD dwMaximumSizeLow,
    _In_opt_ LPCSTR lpName
    );

LPVOID
WINAPI
MapViewOfFileEx(
    _In_ HANDLE hFileMappingObject,
    _In_ DWORD dwDesiredAccess,
    _In_ DWORD dwFileOffsetHigh,
    _In_ DWORD dwFileOffsetLow,
    _In_ SIZE_T dwNumberOfBytesToMap,
    _In_opt_ LPVOID lpBaseAddress
    );


#endif


#else

# include <errno.h>
# include <fcntl.h>
# include <sys/mman.h>      // mmap, munmap.
# include <sys/stat.h>
# include <sys/types.h>     // struct stat.
# include <unistd.h>        // sysconf.
#include "txg_log.h"

#endif

TXCMMapFileParams::TXCMMapFileParams() : flags(static_cast<TXEMMapMode >(0)), offset(0),
                                         length(static_cast<size_t>(-1)),
                                         newFileSize(0), hint(0) {

}

TXCMMapFileParams::~TXCMMapFileParams() {

}

void TXCMMapFileParams::normalize() {
    if (flags) {
        switch (flags) {
            case TXEMMapMode::READ_ONLY:
            case TXEMMapMode::READ_WRITE:
            case TXEMMapMode::PRIV:
                break;
            default:
//				mars_boost::throw_exception(BOOST_IOSTREAMS_FAILURE("invalid flags"));
                break;
        }
    }
//	if (offset < 0) {
////		mars_boost::throw_exception(BOOST_IOSTREAMS_FAILURE("invalid offset"));
//	}
//	if (newFileSize < 0) {
////		mars_boost::throw_exception(BOOST_IOSTREAMS_FAILURE("invalid new file size"));
//	}
}

TXCMMapFile::TXCMMapFile() {
    _clear(false);
}

TXCMMapFile::~TXCMMapFile() {
    close();
}

void TXCMMapFile::open(TXCMMapFileParams p) {
    if (is_open()) {
//		mars_boost::throw_exception(BOOST_IOSTREAMS_FAILURE("file already open"));
        return;
    }
    p.normalize();
    _open_file(p);
    _map_file(p);  // May modify p.hint
    _params = p;
}

bool TXCMMapFile::is_open() const {
    return (_data != 0 && _handle >= 0);
}

void TXCMMapFile::close() {
    if (_data == 0)
        return;
    bool error = false;
    error = !_unmap_file() || error;
#if defined(_WIN32)
    if(_handle != INVALID_HANDLE_VALUE)
#else
    if (_handle >= 0)
#endif
    {
        error =
#if defined(_WIN32)
!::CloseHandle(_handle)
#else
::close(_handle) != 0
#endif
|| error;
    }
    _clear(error);
    if (error) {
//		throw_system_failure("failed closing mapped file");
    }
}

bool TXCMMapFile::error() const {
    return _error;
}

TXCMMapFileParams::TXEMMapMode TXCMMapFile::flags() const {
    return _params.flags;
}

std::size_t TXCMMapFile::size() const {
    return _size;
}

char *TXCMMapFile::data() const {
    return _data;
}

void TXCMMapFile::resize(size_t newSize) {
    if (!is_open()) {
//		mars_boost::throw_exception(BOOST_IOSTREAMS_FAILURE("file is closed"));
        return;
    }
    if (flags() & TXCMMapFileParams::TXEMMapMode::PRIV) {
//		mars_boost::throw_exception(BOOST_IOSTREAMS_FAILURE("can't resize private mapped file"));
        return;
    }
    if (!(flags() & TXCMMapFileParams::TXEMMapMode::READ_WRITE)) {
//		mars_boost::throw_exception(BOOST_IOSTREAMS_FAILURE("can't resize readonly mapped file"));
        return;
    }
    if (_params.offset >= newSize) {
//		mars_boost::throw_exception(BOOST_IOSTREAMS_FAILURE("can't resize below mapped offset"));
        return;
    }
    if (!_unmap_file()) {
        _cleanup("failed unmapping file");
        return;
    }
#if defined(_WIN32)
    stream_offset offset = ::SetFilePointer(_handle, 0, NULL, FILE_CURRENT);
    if (offset == INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR) {
         _cleanup_and_throw("failed querying file pointer");
        return;
    }
    LONG sizehigh = (newSize >> (sizeof(LONG) * 8));
    LONG sizelow = (newSize & 0xffffffff);
    DWORD result = ::SetFilePointer(_handle, sizelow, &sizehigh, FILE_BEGIN);
    if ((result == INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR)
        || !::SetEndOfFile(_handle)) {
        _cleanup_and_throw("failed resizing mapped file");
        return;
    }
    sizehigh = (offset >> (sizeof(LONG) * 8));
    sizelow = (offset & 0xffffffff);
    ::SetFilePointer(_handle, sizelow, &sizehigh, FILE_BEGIN);
#else
    if (TX_FD_TRUNCATE(_handle, newSize) == -1) {
        _cleanup("failed resizing mapped file");
        return;
    }
#endif
    _size = newSize;
    _map_file(_params);
}

int TXCMMapFile::alignment() {
#if defined(_WIN32)
    SYSTEM_INFO info;
    ::GetSystemInfo(&info);
    return static_cast<int>(info.dwAllocationGranularity);
#else
    return static_cast<int>(sysconf(_SC_PAGESIZE));
#endif
}

bool TXCMMapFile::_open_file(TXCMMapFileParams p) {
    bool readonly = p.flags != TXCMMapFileParams::TXEMMapMode::READ_WRITE;
#if defined(_WIN32)
    // Open file
    DWORD dwDesiredAccess =
        readonly ?
            GENERIC_READ :
            (GENERIC_READ | GENERIC_WRITE);
    DWORD dwCreationDisposition = (p.newFileSize != 0 && !readonly) ?
        CREATE_ALWAYS :
        OPEN_EXISTING;
    DWORD dwFlagsandAttributes =
        readonly ?
            FILE_ATTRIBUTE_READONLY :
            FILE_ATTRIBUTE_TEMPORARY;
    _handle = p.path.is_wide() ?
        ::CreateFileW(
            p.path.wstr().c_str(),
            dwDesiredAccess,
            FILE_SHARE_READ,
            NULL,
            dwCreationDisposition,
            dwFlagsandAttributes,
            NULL ) :
        ::CreateFileA(
            p.path.str().c_str(),
            dwDesiredAccess,
            FILE_SHARE_READ,
            NULL,
            dwCreationDisposition,
            dwFlagsandAttributes,
            NULL );
    if (_handle == INVALID_HANDLE_VALUE) {
        _cleanup("failed opening file");
        return false;
    }

    // Set file size
    if (p.newFileSize != 0 && !readonly) {
        LONG sizehigh = (p.newFileSize >> (sizeof(LONG) * 8));
        LONG sizelow = (p.newFileSize & 0xffffffff);
        DWORD result = ::SetFilePointer(_handle, sizelow, &sizehigh, FILE_BEGIN);
        if ((result == INVALID_SET_FILE_POINTER && ::GetLastError() != NO_ERROR) || !::SetEndOfFile(_handle)) {
            _cleanup("failed setting file size");
            return false;
        }
    }

    // Determine file size. Dynamically locate GetFileSizeEx for compatibility
    // with old Platform SDK (thanks to Pavel Vozenilik).
    typedef BOOL (WINAPI *func)(HANDLE, PLARGE_INTEGER);
#ifdef UWP
    func get_size = &GetFileSizeEx;
#else
    HMODULE hmod = ::GetModuleHandleA("kernel32.dll");
    func get_size = reinterpret_cast<func>(::GetProcAddress(hmod, "GetFileSizeEx"));
#endif
    if (get_size) {
        LARGE_INTEGER info;
        if (get_size(_handle, &info)) {
            mars_boost::intmax_t size = ( (static_cast<mars_boost::intmax_t>(info.HighPart) << 32) | info.LowPart );
            _size = static_cast<std::size_t>( p.length != max_length ? std::min<mars_boost::intmax_t>(p.length, size) :size);
        } else {
            cleanup("failed querying file size");
            return false;
        }
    } else {
        DWORD hi;
        DWORD low;
        if ( (low = ::GetFileSize(_handle, &hi)) != INVALID_FILE_SIZE )
        {
            mars_boost::intmax_t size =
                (static_cast<mars_boost::intmax_t>(hi) << 32) | low;
            _size = static_cast<std::size_t>( p.length != max_length ? std::min<mars_boost::intmax_t>(p.length, size) : size);
        } else {
            _cleanup("failed querying file size");
            return false;
        }
    }
#else
    // Open file
    int flags = (readonly ? O_RDONLY : O_RDWR);
    if (p.newFileSize != 0 && !readonly)
        flags |= (O_CREAT | O_TRUNC);
#ifdef _LARGEFILE64_SOURCE
        flags |= O_LARGEFILE;
#endif
    errno = 0;
    _handle = ::open(p.path.str().c_str(), flags, S_IRWXU);
    if (errno != 0) {
        _cleanup("failed opening file");
        return false;
    }

    //--------------Set file size---------------------------------------------//

    if (p.newFileSize != 0 && !readonly)
        if (TX_FD_TRUNCATE(_handle, p.newFileSize) == -1) {
            _cleanup("failed setting file size");
            return false;
        }

    //--------------Determine file size---------------------------------------//

    bool success = true;
    if (p.length != max_length) {
        _size = p.length;
    } else {
        struct TX_FD_STAT info;
        success = ::TX_FD_FSTAT(_handle, &info) != -1;
        _size = info.st_size;
    }
    if (!success) {
        _cleanup("failed querying file size");
        return false;
    }
#endif
    return true;
}

bool TXCMMapFile::_try_map_file(TXCMMapFileParams p) {
    bool priv = p.flags == TXCMMapFileParams::TXEMMapMode::PRIV;
    bool readonly = p.flags == TXCMMapFileParams::TXEMMapMode::READ_ONLY;
#if defined(_WIN32)
    // Create mapping
    DWORD protect = priv ?
        PAGE_WRITECOPY :
        readonly ?
            PAGE_READONLY :
            PAGE_READWRITE;
    _mapped_handle =
        ::CreateFileMappingA(
            _handle,
            NULL,
            protect,
            0,
            0,
            NULL );
    if (_mapped_handle == NULL)
        _cleanup("failed create mapping");

    // Access data
    DWORD access = priv ?
        FILE_MAP_COPY :
        readonly ?
            FILE_MAP_READ :
            FILE_MAP_WRITE;
    void* data =
        ::MapViewOfFileEx(
            _mapped_handle,
            access,
            (DWORD) (p.offset >> 32),
            (DWORD) (p.offset & 0xffffffff),
            _size != max_length ? _size : 0,
            (LPVOID) p.hint );
    if (!data) {
        _cleanup("failed mapping view");
        return false;
    }
#else
    void *data =
            ::TX_FD_MMAP(
                    const_cast<char *>(p.hint),
                    _size,
                    readonly ? PROT_READ : (PROT_READ | PROT_WRITE),
                    priv ? MAP_PRIVATE : MAP_SHARED,
                    _handle,
                    p.offset);
    if (data == MAP_FAILED) {
        _cleanup("failed mapping file");
        return false;
    }
#endif
    _data = static_cast<char *>(data);
    return true;
}

bool TXCMMapFile::_map_file(TXCMMapFileParams &p) {
    if(!_try_map_file(p)){
        if (p.hint) {
            p.hint = 0;
            return _try_map_file(p);
        } else {
            return false;
        }
    }
    return true;
}

bool TXCMMapFile::_unmap_file() {
#if defined(_WIN32)
    bool error = false;
    error = !::UnmapViewOfFile(_data) || error;
    error = !::CloseHandle(_mapped_handle) || error;
    _mapped_handle = NULL;
    return !error;
#else
    return ::munmap(_data, _size) == 0;
#endif
}

void TXCMMapFile::_clear(bool error) {
    _params = TXCMMapFileParams();
    _data = 0;
    _size = 0;
#if defined(_WIN32)
    _handle = INVALID_HANDLE_VALUE;
    _mapped_handle = NULL;
#else
    _handle = -1;
#endif
    _error = error;
}

void TXCMMapFile::_cleanup(const char *msg) {
#if defined(_WIN32)
    DWORD error = GetLastError();
    if (_mapped_handle != NULL)
        ::CloseHandle(_mapped_handle);
    if (_handle != INVALID_HANDLE_VALUE)
        ::CloseHandle(_handle);
    SetLastError(error);
#else
    int error = errno;
    if (_handle >= 0)
        ::close(_handle);
    errno = error;
#endif
    _clear(true);
    LOGE("%s", msg);
}
