//
// Created by alderzhang on 2017/5/19.
//

#include "TXCPath.h"
#include <sstream>
#include <sys/stat.h>

#if !defined(_WIN32)

# include <unistd.h>

#endif

#if defined(__linux)

# include <linux/limits.h>
#include <cstdio>
#include <stdlib.h>

#endif

#ifdef _WIN32
#define PATH_SEPARATOR   '\\'
#else
#define PATH_SEPARATOR   '/'
#endif

char *path_normalize(const char *path) {
    if (!path) return NULL;

    char *copy = strdup(path);
    if (NULL == copy) return NULL;
    char *ptr = copy;

    for (int i = 0; copy[i]; i++) {
        *ptr++ = path[i];
        if ('/' == path[i]) {
            i++;
            while ('/' == path[i]) i++;
            i--;
        }
    }

    *ptr = '\0';

    return copy;
}

/*
 * Recursively `mkdir(path, mode)`
 */

int mkdirp(const char *path, mode_t mode) {
    char *pathname = NULL;
    char *parent = NULL;
    int rc;
    char *p;

    if (NULL == path) return -1;

    pathname = path_normalize(path);
    if (NULL == pathname) goto fail;

    parent = strdup(pathname);
    if (NULL == parent) goto fail;

    p = parent + strlen(parent);
    while (PATH_SEPARATOR != *p && p != parent) {
        p--;
    }
    *p = '\0';

    // make parent dir
    if (p != parent && 0 != mkdirp(parent, mode)) goto fail;
    free(parent);

    // make this one if parent has been made
#ifdef _WIN32
    // http://msdn.microsoft.com/en-us/library/2fkk4dzw.aspx
    rc = mkdir(pathname);
#else
    rc = mkdir(pathname, mode);
#endif

    free(pathname);

    return 0 == rc || EEXIST == errno
           ? 0
           : -1;

    fail:
    free(pathname);
    free(parent);
    return -1;
}

TXCPath::TXCPath()
        : _type(native_path), _wide(false), _absolute(false) {

}

TXCPath::TXCPath(const TXCPath &path)
        : _type(path._type), _wide(path._wide), _absolute(path._absolute), _path(path._path) {

}

TXCPath::TXCPath(TXCPath &&path)
        : _type(path._type), _wide(path._wide), _absolute(path._absolute),
          _path(std::move(path._path)) {

}

TXCPath::TXCPath(const char *string) {
    set(string);
}

TXCPath::TXCPath(const std::string &string) {
    set(string);
}

TXCPath::~TXCPath() {

}

size_t TXCPath::length() const {
    return _path.size();
}

bool TXCPath::empty() const {
    return _path.empty();
}

bool TXCPath::is_absolute() const {
    return _absolute;
}

TXCPath TXCPath::make_absolute() const {
#if !defined(_WIN32)
    char temp[PATH_MAX];
    if (realpath(str().c_str(), temp) == NULL)
        return TXCPath();
//		throw std::runtime_error("Internal error in realpath(): " + std::string(strerror(errno)));
    return TXCPath(temp);
#else
    std::wstring value = wstr(), out(MAX_PATH, '\0');
    DWORD length = GetFullPathNameW(value.c_str(), MAX_PATH, &out[0], NULL);
    if (length == 0)
        return TXCPath();
//		throw std::runtime_error("Internal error in realpath(): " + std::to_string(GetLastError()));
    return TXCPath(out.substr(0, length));
#endif
}

bool TXCPath::exists() const {
#if defined(_WIN32)
    return GetFileAttributesW(wstr().c_str()) != INVALID_FILE_ATTRIBUTES;
#else
    struct stat sb;
    return stat(str().c_str(), &sb) == 0;
#endif
}

size_t TXCPath::file_size() const {
#if defined(_WIN32)
    struct _stati64 sb;
    if (_wstati64(wstr().c_str(), &sb) != 0)
        return 0;
//		throw std::runtime_error("TXCPath::file_size(): cannot stat file \"" + str() + "\"!");
#else
    struct stat sb;
    if (stat(str().c_str(), &sb) != 0)
        return 0;
//		throw std::runtime_error("TXCPath::file_size(): cannot stat file \"" + str() + "\"!");
#endif
    return (size_t) sb.st_size;
}

bool TXCPath::is_directory() const {
#if defined(_WIN32)
    DWORD attr = GetFileAttributesW(wstr().c_str());
        return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
    struct stat sb;
    if (stat(str().c_str(), &sb))
        return false;
    return S_ISDIR(sb.st_mode);
#endif
}

bool TXCPath::is_file() const {
#if defined(_WIN32)
    DWORD attr = GetFileAttributesW(wstr().c_str());
        return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;
#else
    struct stat sb;
    if (stat(str().c_str(), &sb))
        return false;
    return S_ISREG(sb.st_mode);
#endif
}

std::string TXCPath::extension() const {
    const std::string &name = filename();
    size_t pos = name.find_last_of(".");
    if (pos == std::string::npos)
        return "";
    return name.substr(pos + 1);
}

std::string TXCPath::filename() const {
    if (empty())
        return "";
    const std::string &last = _path[_path.size() - 1];
    return last;
}

TXCPath TXCPath::parent_path() const {
    TXCPath result;
    result._absolute = _absolute;

    if (_path.empty()) {
        if (!_absolute)
            result._path.push_back("..");
    } else {
        size_t until = _path.size() - 1;
        for (size_t i = 0; i < until; ++i)
            result._path.push_back(_path[i]);
    }
    return result;
}

time_t TXCPath::creation_time() const {
    return last_status_change_time();
}

time_t TXCPath::last_access_time() const {
#if defined(_WIN32)
    struct _stati64 sb;
    if (_wstati64(wstr().c_str(), &sb) != 0)
        return 0;
//		throw std::runtime_error("TXCPath::last_access_time(): cannot stat file \"" + str() + "\"!");
#else
    struct stat sb;
    if (stat(str().c_str(), &sb) != 0)
        return 0;
//		throw std::runtime_error("TXCPath::last_access_time(): cannot stat file \"" + str() + "\"!");
#endif
    return sb.st_atime;
}

time_t TXCPath::last_modified_time() const {
#if defined(_WIN32)
    struct _stati64 sb;
    if (_wstati64(wstr().c_str(), &sb) != 0)
        return 0;
//		throw std::runtime_error("TXCPath::last_modified_time(): cannot stat file \"" + str() + "\"!");
#else
    struct stat sb;
    if (stat(str().c_str(), &sb) != 0)
        return 0;
//		throw std::runtime_error("TXCPath::last_modified_time(): cannot stat file \"" + str() + "\"!");
#endif
    return sb.st_mtime;
}

time_t TXCPath::last_status_change_time() const {
#if defined(_WIN32)
    struct _stati64 sb;
    if (_wstati64(wstr().c_str(), &sb) != 0)
        return 0;
//		throw std::runtime_error("TXCPath::last_status_change_time(): cannot stat file \"" + str() + "\"!");
#else
    struct stat sb;
    if (stat(str().c_str(), &sb) != 0)
        return 0;
//		throw std::runtime_error("TXCPath::last_status_change_time(): cannot stat file \"" + str() + "\"!");
#endif
    return sb.st_ctime;
}

std::string TXCPath::str(TXCPath::path_type type) const {
    std::ostringstream oss;
    if (_type == posix_path && _absolute)
        oss << "/";
    for (size_t i = 0; i < _path.size(); ++i) {
        oss << _path[i];
        if (i + 1 < _path.size()) {
            if (type == posix_path)
                oss << '/';
            else
                oss << '\\';
        }
    }
    return oss.str();
}

void TXCPath::set(const std::string &str, TXCPath::path_type type) {
    _type = type;
    if (type == windows_path) {
        _path = tokenize(str, "/\\");
        _absolute = str.size() >= 2 && std::isalpha(str[0]) && str[1] == ':';
    } else {
        _path = tokenize(str, "/");
        _absolute = !str.empty() && str[0] == '/';
    }
    _wide = false;
}

TXCPath TXCPath::operator/(const TXCPath &other) const {
//	if (other._absolute)
//		throw std::runtime_error("TXCPath::operator/(): expected a relative path!");
//	if (_type != other._type)
//		throw std::runtime_error("TXCPath::operator/(): expected a path of the same type!");
    TXCPath result(*this);
    for (size_t i = 0; i < other._path.size(); ++i)
        result._path.push_back(other._path[i]);
    return result;
}

TXCPath &TXCPath::operator=(const TXCPath &path) {
    _type = path._type;
    _path = path._path;
    _absolute = path._absolute;
    return *this;
}

TXCPath &TXCPath::operator=(TXCPath &&path) {
    if (this != &path) {
        _type = path._type;
        _path = std::move(path._path);
        _absolute = path._absolute;
    }
    return *this;
}

std::ostream &operator<<(std::ostream &os, const TXCPath &path) {
    os << path.str();
    return os;
}

bool TXCPath::operator==(const TXCPath &p) const {
    return p._path == _path;
}

bool TXCPath::operator!=(const TXCPath &p) const {
    return p._path != _path;
}

bool TXCPath::remove_file() const {
#if defined(_WIN32)
    return DeleteFileW(wstr().c_str()) != 0;
#else
    return std::remove(str().c_str()) == 0;
#endif
}

bool TXCPath::resize_file(size_t target_length) const {
#if defined(_WIN32)
    HANDLE handle = CreateFileW(wstr().c_str(), GENERIC_WRITE, 0, nullptr, 0, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (handle == INVALID_HANDLE_VALUE)
        return false;
    LARGE_INTEGER size;
    size.QuadPart = (LONGLONG) target_length;
    if (SetFilePointerEx(handle, size, NULL, FILE_BEGIN) == 0) {
        CloseHandle(handle);
        return false;
    }
    if (SetEndOfFile(handle) == 0) {
        CloseHandle(handle);
        return false;
    }
    CloseHandle(handle);
    return true;
#else
    return ::truncate(str().c_str(), (off_t) target_length) == 0;
#endif
}

bool TXCPath::create_directory() const {
    return mkdirp(str().c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == 0;
}

TXCPath TXCPath::getcwd() {
#if !defined(_WIN32)
    char temp[PATH_MAX];
    if (::getcwd(temp, PATH_MAX) == NULL)
        return TXCPath();
//		throw std::runtime_error("Internal error in getcwd(): " + std::string(strerror(errno)));
    return TXCPath(temp);
#else
    std::wstring temp(MAX_PATH, '\0');
    if (!_wgetcwd(&temp[0], MAX_PATH))
        return TXCPath();
//		throw std::runtime_error("Internal error in getcwd(): " + std::to_string(GetLastError()));
    return TXCPath(temp.c_str());
#endif
}

std::vector<std::string> TXCPath::tokenize(const std::string &string, const std::string &delim) {
    std::string::size_type lastPos = 0, pos = string.find_first_of(delim, lastPos);
    std::vector<std::string> tokens;

    while (lastPos != std::string::npos) {
        if (pos != lastPos)
            tokens.push_back(string.substr(lastPos, pos - lastPos));
        lastPos = pos;
        if (lastPos == std::string::npos || lastPos + 1 == string.length())
            break;
        pos = string.find_first_of(delim, ++lastPos);
    }

    return tokens;
}

bool TXCPath::is_wide() {
    return _wide;
}

#if defined(_WIN32)
TXCPath::TXCPath(const wchar_t *wstring) {
    set(wstring);
}

TXCPath::TXCPath(const std::wstring &wstring) {
    set(wstring);
}

std::wstring TXCPath::wstr(path_type type = native_path) const {
    std::string temp = str(type);
    int size = MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int) temp.size(), NULL, 0);
    std::wstring result(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int) temp.size(), &result[0], size);
    return result;
}


void TXCPath::set(const std::wstring &wstring, path_type type = native_path) {
    std::string string;
    if (!wstring.empty()) {
        int size = WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int) wstring.size(),
                                       NULL, 0, NULL, NULL);
        string.resize(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstring[0], (int) wstring.size(),
                            &string[0], size, NULL, NULL);
    }
    set(string, type);
    _wide = true;
}

TXCPath &TXCPath::operator=(const std::wstring &str) {
    set(str);
    return *this;
}
#endif
