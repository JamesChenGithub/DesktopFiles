/*
 * loginfo_extract.c
 *
 *  Created on: 2016年10月13日
 *      Author: yanguoyue
 */

#include "txg_log_util.h"

#include <string.h>
#include <stddef.h>


const char *txf_extract_file_name(const char *path) {
	if (NULL == path) return "";

	const char *pos = strrchr(path, '\\');

	if (NULL == pos) {
		pos = strrchr(path, '/');
	}

	if (NULL == pos || '\0' == *(pos + 1)) {
		return path;
	} else {
		return pos + 1;
	}
}


void txf_extract_function_name(const char *func, char *func_ret, int len) {
	if (NULL == func)return;

	const char *start = func;
	const char *end = NULL;
	const char *pos = func;

	while ('\0' != *pos) {
		if (NULL == end && ' ' == *pos) {
			start = ++pos;
			continue;
		}

		if (':' == *pos && ':' == *(pos + 1)) {
			pos += 2;
			start = pos;
			continue;
		}

		if ('(' == *pos) {
			end = pos;
		} else if (NULL != start && (':' == *pos || ']' == *pos)) {
			end = pos;
			break;
		}
		++pos;
	}


	if (NULL == start || NULL == end || start + 1 >= end) {
		strncpy(func_ret, func, len);
		func_ret[len - 1] = '\0';
		return;
	}

	ptrdiff_t nLen = end - start;
	--len;
	nLen = len < nLen ? len : nLen;
	memcpy(func_ret, start, nLen);
	func_ret[nLen] = '\0';
}
