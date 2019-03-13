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
 ============================================================================
 ============================================================================
 */

#ifndef TXG_LOG_BASE_H
#define TXG_LOG_BASE_H

#include <sys/time.h>
#include <time.h>
#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	kLevelAll = 0,
	kLevelVerbose = 0,
	kLevelDebug,    // Detailed information on the flow through the system.
	kLevelInfo,     // Interesting runtime events (startup/shutdown), should be conservative and keep to a minimum.
	kLevelWarn,     // Other runtime situations that are undesirable or unexpected, but not necessarily "wrong".
	kLevelError,    // Other runtime errors or unexpected conditions.
	kLevelFatal,    // Severe errors that cause premature termination.
	kLevelNone,     // Special level used to disable all log messages.
} TXELoggerLevel;

typedef struct TXSLogInfo_t {
	TXELoggerLevel level;
	const char *tag;
	const char *filename;
	const char *func_name;
	int line;

	struct timeval timeval;
	intmax_t pid;
	intmax_t tid;
	intmax_t maintid;
} TXSLogInfo;

extern intmax_t txf_logger_pid();

extern intmax_t txf_logger_tid();

extern intmax_t txf_logger_main_tid();

typedef void (*txf_logger_appender_t)(const TXSLogInfo *info, const char *log);

extern const char *txf_logger_dump(const void *dumpbuffer, size_t len);

TXELoggerLevel txf_logger_level();

void txf_logger_set_level(TXELoggerLevel level);

int txf_logger_is_enabled_for(TXELoggerLevel level);

txf_logger_appender_t txf_logger_set_appender(txf_logger_appender_t appender);

// no level filter
#ifdef __GNUC__

__attribute__((__format__(printf, 3, 4)))
#endif
void txf_logger_assert_p(const TXSLogInfo *info, const char *expression, const char *format, ...);

void txf_logger_assert(const TXSLogInfo *info, const char *expression, const char *log);

#ifdef __GNUC__

__attribute__((__format__(printf, 2, 0)))
#endif
void txf_logger_vprint(const TXSLogInfo *info, const char *format, va_list list);

#ifdef __GNUC__

__attribute__((__format__(printf, 2, 3)))
#endif
void txf_logger_print(const TXSLogInfo *info, const char *format, ...);

void txf_logger_write(const TXSLogInfo *info, const char *log);

#ifdef __cplusplus
}
#endif

#endif // TXG_LOG_BASE_H
