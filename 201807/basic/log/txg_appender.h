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
 * appender.h
 *
 *  Created on: 2013-3-7
 *      Author: yerungui
 */

#ifndef TXG_APPENDER_H
#define TXG_APPENDER_H

#include <string>
#include <vector>

enum TXEAppenderMode {
	kAppednerAsync,
	kAppednerSync,
};

void txf_appender_open(TXEAppenderMode mode, const char *dir, const char *nameprefix);

void txf_appender_open_with_cache(TXEAppenderMode mode, const std::string &cachedir, const std::string &logdir,
								  const char *nameprefix);

void txf_appender_flush();

void txf_appender_flush_sync();

void txf_appender_close();

void txf_appender_setmode(TXEAppenderMode mode);

bool
txf_appender_getfilepath_from_timespan(int timespan, const char *prefix, std::vector<std::string> &filepath_vec);

bool txf_appender_get_current_log_path(char *log_path, unsigned int len);

bool txf_appender_get_current_log_cache_path(char *logPath, unsigned int len);

void txf_appender_set_console_log(bool is_open);


#endif /* TXG_APPENDER_H */
