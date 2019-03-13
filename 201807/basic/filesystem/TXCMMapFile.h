//
// Created by alderzhang on 2017/5/19.
//

#ifndef TXCMMAPFILE_H
#define TXCMMAPFILE_H

#if defined(_WIN32)
#include <windows.h>
#else

#include <unistd.h>

#endif

#include "TXCPath.h"

#if defined(_WIN32)
typedef void*  file_handle;  // A.k.a. HANDLE
#else
typedef int file_handle;
#endif

#if defined(_WIN32)
# if defined(_MSC_VER) || defined(__MSVCRT__) // MSVC, MinGW
#  define TX_FD_SEEK    _lseeki64
#  define TX_FD_OFFSET  __int64
# else                                          // Borland, Metrowerks, ...
#  define TX_FD_SEEK    lseek
#  define TX_FD_OFFSET  long
# endif
#else // Non-windows
# if defined(_LARGEFILE64_SOURCE) && !defined(__APPLE__) && \
         (!defined(_FILE_OFFSET_BITS) || _FILE_OFFSET_BITS != 64) || \
     defined(_AIX) && !defined(_LARGE_FILES)
/**/

	/* Systems with transitional extensions for large file support */

#  define TX_FD_SEEK      lseek64
#  define TX_FD_TRUNCATE  ftruncate64
#  define TX_FD_MMAP      mmap64
#  define TX_FD_STAT      stat64
#  define TX_FD_FSTAT     fstat64
#  define TX_FD_OFFSET    off64_t
# else
#  define TX_FD_SEEK      lseek
#  define TX_FD_TRUNCATE  ftruncate
#  define TX_FD_MMAP      mmap
#  define TX_FD_STAT      stat
#  define TX_FD_FSTAT     fstat
#  define TX_FD_OFFSET    off_t
# endif
#endif

class TXCMMapFileParams {
public:
	enum TXEMMapMode {
		READ_ONLY = 1,
		READ_WRITE = 2,
		PRIV = 4
	};

	TXCPath path;
	TXEMMapMode flags;
	size_t offset;
	size_t length;
	size_t newFileSize;
	const char *hint;

	TXCMMapFileParams();

	~TXCMMapFileParams();

	void normalize();
};

class TXCMMapFile {
public:
	const static size_t max_length = static_cast<size_t >(-1);

	TXCMMapFile();

	~TXCMMapFile();

	void open(TXCMMapFileParams p);

	bool is_open() const;

	void close();

	bool error() const;

	TXCMMapFileParams::TXEMMapMode flags() const;

	std::size_t size() const;

	char *data() const;

	void resize(size_t newSize);

	static int alignment();

private:
	bool _open_file(TXCMMapFileParams p);

	bool _try_map_file(TXCMMapFileParams p);

	bool _map_file(TXCMMapFileParams &p);

	bool _unmap_file();

	void _clear(bool error);

	void _cleanup(const char *msg);

	char *_data;

	TXCMMapFileParams _params;

	size_t _size;

	file_handle _handle;

#if defined(_WIN32)
	file_handle 			_mapped_handle;
#endif

	bool _error;
};

#endif //TXCMMAPFILE_H
