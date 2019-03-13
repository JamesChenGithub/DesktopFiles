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
 * txf_log_formater.cpp
 *
 *  Created on: 2013-3-8
 *      Author: yerungui
 */


#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <algorithm>

#include "txg_log_base.h"
#include "txg_log_util.h"
#include "TXCPtrBuffer.h"

#ifdef _WIN32
#define PRIdMAX "lld"
#define snprintf _snprintf
#else
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#endif

void txf_log_formater(const TXSLogInfo *info, const char *logbody, TXCPtrBuffer &log) {
    static const char* levelStrings[] = {
        "V",
        "D",  // debug
        "I",  // info
        "W",  // warn
        "E",  // error
        "F"  // fatal
    };

    assert((unsigned int)log.Pos() == log.Length());

    static int error_count = 0;
    static int error_size = 0;

    if (log.MaxLength() <= log.Length() + 5 * 1024) {  // allowd len(log) <= 11K(16K - 5K)
        ++error_count;
        error_size = (int)strnlen(logbody, 1024 * 1024);

        if (log.MaxLength() >= log.Length() + 128) {
            int ret = snprintf((char*)log.PosPtr(), 1024, "[F]log_size <= 5*1024, err(%d, %d)\n", error_count, error_size);  // **CPPLINT SKIP**
            log.Length(log.Pos() + ret, log.Length() + ret);
            log.Write("");

            error_count = 0;
            error_size = 0;
        }

        assert(false);
        return;
    }

    if (NULL != info) {
        const char* filename = txf_extract_file_name(info->filename);
        char strFuncName [128] = {0};
		txf_extract_function_name(info->func_name, strFuncName, sizeof(strFuncName));

        char temp_time[64] = {0};

        if (0 != info->timeval.tv_sec) {
            time_t sec = info->timeval.tv_sec;
            tm tm = *localtime((const time_t*)&sec);
#ifdef ANDROID
            snprintf(temp_time, sizeof(temp_time), "%d-%02d-%02d %+.1f %02d:%02d:%02d.%.3ld", 1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday,
                     tm.tm_gmtoff / 3600.0, tm.tm_hour, tm.tm_min, tm.tm_sec, info->timeval.tv_usec / 1000);
#elif _WIN32
            snprintf(temp_time, sizeof(temp_time), "%d-%02d-%02d %+.1f %02d:%02d:%02d.%.3d", 1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday,
                     (-_timezone) / 3600.0, tm.tm_hour, tm.tm_min, tm.tm_sec, info->timeval.tv_usec / 1000);
#else
            snprintf(temp_time, sizeof(temp_time), "%d-%02d-%02d %+.1f %02d:%02d:%02d.%.3d", 1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday,
                     tm.tm_gmtoff / 3600.0, tm.tm_hour, tm.tm_min, tm.tm_sec, info->timeval.tv_usec / 1000);
#endif
        }

        // log.AllocWrite(30*1024, false);
        int ret = snprintf((char*)log.PosPtr(), 1024, "[%s][%s][%" PRIdMAX ", %" PRIdMAX "%s][%s][%s, %s, %d][",  // **CPPLINT SKIP**
                           logbody ? levelStrings[info->level] : levelStrings[kLevelFatal], temp_time,
                           info->pid, info->tid, info->tid == info->maintid ? "*" : "", info->tag ? info->tag : "",
                           filename, strFuncName, info->line);

        assert(0 <= ret);
        log.Length(log.Pos() + ret, log.Length() + ret);
        //      memcpy((char*)log.PosPtr() + 1, "\0", 1);

        assert((unsigned int)log.Pos() == log.Length());
    }

    if (NULL != logbody) {
        // in android 64bit, in strnlen memchr,  const unsigned char*  end = p + n;  > 4G!!!!! in stack array

        size_t bodylen =  log.MaxLength() - log.Length() > 130 ? log.MaxLength() - log.Length() - 130 : 0;
        bodylen = bodylen > 0xFFFFU ? 0xFFFFU : bodylen;
        bodylen = strnlen(logbody, bodylen);
        bodylen = bodylen > 0xFFFFU ? 0xFFFFU : bodylen;
        log.Write(logbody, bodylen);
    } else {
        log.Write("error!! NULL==logbody");
    }

    char nextline = '\n';

    if (*((char*)log.PosPtr() - 1) != nextline) log.Write(&nextline, 1);
}

