//
// Created by 张圣辉 on 2017/5/21.
//
#include <zconf.h>
#include "log/txg_log_util.h"
#include "log/TXCLogger.h"

void txf_console_log(const TXSLogInfo *info, const char *log) {
	if (NULL == info || NULL == log) return;

	static const char *levelStrings[] = {
			"V",
			"D",  // debug
			"I",  // info
			"W",  // warn
			"E",  // error
			"F"  // fatal
	};

	char strFuncName[128] = {0};
	txf_extract_function_name(info->func_name, strFuncName, sizeof(strFuncName));

	const char *file_name = txf_extract_file_name(info->filename);

	char logBuff[2048] = {0};
	snprintf(logBuff, sizeof(logBuff), "[%s][%s][%s, %s, %d][%s", levelStrings[info->level],
			 NULL == info->tag ? "" : info->tag, file_name, strFuncName, info->line, log);


	printf("%s\n", logBuff);
}

extern "C"
{
intmax_t txf_logger_pid() {
	return getpid();
}

intmax_t txf_logger_tid() {
	return 0;
}

intmax_t txf_logger_main_tid() {
	return 0;
}
}