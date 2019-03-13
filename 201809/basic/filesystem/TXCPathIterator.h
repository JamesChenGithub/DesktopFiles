//
// Created by alderzhang on 2017/5/19.
//

#ifndef TXCPATHITERATOR_H
#define TXCPATHITERATOR_H

#include "TXCPath.h"

#if defined(_WIN32)
# include <windows.h>
#else
# include <dirent.h>
#endif

class TXCPathIterator {
public:
	TXCPathIterator(TXCPath dirPath);

	~TXCPathIterator();

	bool valid();

	const TXCPath& path();

	TXCPathIterator next();

private:


	bool _fetchNextValid();

private:
#if defined(_WIN32)
	HANDLE _handle;

	TXCPathIterator(TXCPath dirPath, HANDLE handle);
#else
	DIR *_dir;

	TXCPathIterator(TXCPath dirPath, DIR *dir);
#endif
	TXCPath _path;
};


#endif //TXCPATHITERATOR_H
