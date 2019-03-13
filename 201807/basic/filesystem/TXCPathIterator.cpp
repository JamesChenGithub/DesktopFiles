//
// Created by alderzhang on 2017/5/19.
//

#include "TXCPathIterator.h"

TXCPathIterator::TXCPathIterator(TXCPath dirPath) {
#if defined(_WIN32)
	_handle = INVALID_HANDLE_VALUE;
#else
	_dir = NULL;
#endif
	_path = dirPath;
	_fetchNextValid();
}

TXCPathIterator::~TXCPathIterator() {

}

bool TXCPathIterator::valid() {
#if defined(_WIN32)
	return _handle != INVALID_HANDLE_VALUE;
#else
	return _dir != NULL;
#endif
}

const TXCPath &TXCPathIterator::path() {
	return _path;
}

TXCPathIterator TXCPathIterator::next() {
#if defined(_WIN32)
	return TXCPathIterator(_path.parent_path(), _handle);
#else
	return TXCPathIterator(_path.parent_path(), _dir);
#endif
}

bool TXCPathIterator::_fetchNextValid() {
#if defined(_WIN32)
	WIN32_FIND_DATAW fileData;
	if(_handle == INVALID_HANDLE_VALUE){
		_handle = FindFirstFileW((_path.wstr() + L"\\*").c_str(), &fileData);
	}
	else{
		if(!FindNextFileW(_handle, &fileData)){
			CloseHandle(_handle);
			_handle = INVALID_HANDLE_VALUE;
		}
	}
	while(_handle != INVALID_HANDLE_VALUE){
		if (_fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			if(wcscmp(fileData.cFileName, L".") == 0 || wcscmp(fileData.cFileName, L"..") == 0){
				if(FindNextFileW(_handle, &fileData)){
					continue;
				}
				else{
					CloseHandle(_handle);
					_handle = INVALID_HANDLE_VALUE;
				}
			}
		}
		break;
    }
    if(_handle != INVALID_HANDLE_VALUE){
		_path = _path / fileData.cFileName;
		return true;
    }
#else
	dirent *file = NULL;
	if(_dir == NULL){
		_dir = opendir(_path.str().c_str());
	}
	while(_dir != NULL){
		file = readdir(_dir);
		if(file != NULL){
			if(file->d_type != DT_DIR || (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0)){
				break;
			}
		}
		else{
			closedir(_dir);
			_dir = NULL;
		}
	}
	if(_dir != NULL){
		_path = _path / file->d_name;
		return true;
	}
#endif
	return false;
}

#if defined(_WIN32)
TXCPathIterator::TXCPathIterator(TXCPath dirPath, HANDLE handle){
	_handle = handle;
	_path = dirPath;
	_fetchNextValid();
}
#else
TXCPathIterator::TXCPathIterator(TXCPath dirPath, DIR *dir) {
	_dir = dir;
	_path = dirPath;
	_fetchNextValid();
}
#endif
