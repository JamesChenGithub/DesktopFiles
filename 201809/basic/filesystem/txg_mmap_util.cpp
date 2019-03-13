// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in 
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.

/*
 * mmap_util.c
 *
 *  Created on: 2016-2-22
 *      Author: yanguoyue
 */

#include "txg_mmap_util.h"

#include <unistd.h>
#include <stdio.h>

#include "TXCMMapFile.h"

bool txf_is_mmap_file_open_success(const TXCMMapFile &mmap_file) {
	return mmap_file.is_open();
}

bool txf_open_mmap_file(const char *filepath, unsigned int size, TXCMMapFile &mmap_file) {

	if (NULL == filepath || 0 == strnlen(filepath, 128) || 0 == size) {
		return false;
	}

	if (txf_is_mmap_file_open_success(mmap_file)) {
		txf_close_mmap_file(mmap_file);
	}

	if (mmap_file.is_open()) {
		return false;
	}

	TXCMMapFileParams param;
	param.path = filepath;
	param.flags = TXCMMapFileParams::TXEMMapMode::READ_WRITE;

	TXCPath path(filepath);
	bool file_exist = path.exists();
	if (!file_exist) {
		param.newFileSize = size;
	}

	mmap_file.open(param);

	bool is_open = txf_is_mmap_file_open_success(mmap_file);

	if (!file_exist && is_open) {

		//Extending a file with ftruncate, thus creating a big hole, and then filling the hole by mod-ifying a shared mmap() can lead to SIGBUS when no space left
		//the boost library uses ftruncate, so we pre-allocate the file's backing store by writing zero.
		FILE *file = fopen(filepath, "rb+");
		if (NULL == file) {
			mmap_file.close();
			remove(filepath);
			return false;
		}

		char *zero_data = new char[size];
		memset(zero_data, 0, size);

		if (size != fwrite(zero_data, sizeof(char), size, file)) {
			mmap_file.close();
			fclose(file);
			remove(filepath);
			delete[] zero_data;
			return false;
		}
		fclose(file);
		delete[] zero_data;
	}

	return is_open;
}

void txf_close_mmap_file(TXCMMapFile &mmap_file) {
	if (mmap_file.is_open()) {
		mmap_file.close();
	}
}
