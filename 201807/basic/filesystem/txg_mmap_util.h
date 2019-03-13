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
 * mmap_util.h
 *
 *  Created on: 2016-2-22
 *      Author: yanguoyue
 */

#ifndef TXG_MMAP_UTIL_H
#define TXG_MMAP_UTIL_H

#include "TXCMMapFile.h"

bool txf_is_mmap_file_open_success(const TXCMMapFile &mmap_file);

bool txf_open_mmap_file(const char *filepath, unsigned int size, TXCMMapFile &mmap_file);

void txf_close_mmap_file(TXCMMapFile &mmap_file);


#endif /* TXG_MMAP_UTIL_H */
