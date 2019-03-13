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

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "txg_appender.h"
#include <stdio.h>

#ifdef _WIN32
#define PRIdMAX "lld"
#define snprintf _snprintf
#define strcasecmp _stricmp
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define fileno _fileno
#else
#define __STDC_FORMAT_MACROS

#include <inttypes.h>
#include <sys/mount.h>

#endif

#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <zlib.h>

#include <string>
#include <algorithm>
#include <functional>

#include "txg_verinfo.h"

#include "TXCLock.h"
#include "TXCCondition.h"
#include "TXCThread.h"
#include "TXCThreadLocalStore.h"

#include "TXCTickCount.h"
#include "TXCTickCount.h"
#include "txg_bootrun.h"
#include "txg_time_util.h"
#include "txg_str_util.h"
#include "txg_log_base.h"

#include "txg_mmap_util.h"
#include "TXCPathIterator.h"
#include "TXCMMapFile.h"

#include "TXCAutoBuffer.h"
#include "TXCPtrBuffer.h"
#include "TXCLogBuffer.h"

#define LOG_EXT "xlog"

extern void txf_log_formater(const TXSLogInfo *info, const char *logbody, TXCPtrBuffer &log);

extern void txf_console_log(const TXSLogInfo *info, const char *log);

static TXEAppenderMode txv_mode = kAppednerAsync;

static std::string txv_logdir;
static std::string txv_cache_logdir;
static std::string txv_logfileprefix;

static TXCMutex txv_mutex_log_file;
static FILE *txv_logfile = NULL;
static time_t txv_openfiletime = 0;
static std::string txv_current_dir;

static TXCMutex txv_mutex_buffer_async;
#ifdef _WIN32
static TXCCondition& txv_cond_buffer_async = *(new TXCCondition());  // 改成引用, 避免在全局释放时执行析构导致crash
#else
static TXCCondition txv_cond_buffer_async;
#endif

static TXCLogBuffer *txv_log_buff = NULL;

static volatile bool txv_log_close = true;

static TXCThreadLocalStore txv_tss_dumpfile(&free);

#ifdef DEBUG
static bool txv_consolelog_open = true;
#else
static bool txv_consolelog_open = false;
#endif

static void __async_log_thread();

static TXCThread txv_thread_async(&__async_log_thread);

static const unsigned int kBufferBlockLength = 150 * 1024;
static const long kMaxLogAliveTime = 10 * 24 * 60 * 60;    // 10 days in second

static std::string txv_log_extra_msg;

static TXCMMapFile txv_mmap_file;

namespace {
	class TXCScopeErrno {
	public:
		TXCScopeErrno() { _errno = errno; }

		~TXCScopeErrno() { errno = _errno; }

	private:
		TXCScopeErrno(const TXCScopeErrno &);

		const TXCScopeErrno &operator=(const TXCScopeErrno &);

	private:
		int _errno;
	};

#define SCOPE_ERRNO() SCOPE_ERRNO_I(__LINE__)
#define SCOPE_ERRNO_I(line) SCOPE_ERRNO_II(line)
#define SCOPE_ERRNO_II(line) TXCScopeErrno __scope_errno_##line

}

static void
__make_logfilename(const timeval &tv, const std::string &logdir, const char *prefix, const std::string &fileext,
				   char *filepath, unsigned int len) {
	time_t sec = tv.tv_sec;
	tm tcur = *localtime((const time_t *) &sec);

	std::string logfilepath = logdir;
	logfilepath += "/";
	logfilepath += prefix;
	char temp[64] = {0};
	snprintf(temp, 64, "_%d%02d%02d", 1900 + tcur.tm_year, 1 + tcur.tm_mon, tcur.tm_mday);
	logfilepath += temp;
	logfilepath += ".";
	logfilepath += fileext;
	strncpy(filepath, logfilepath.c_str(), len - 1);
	filepath[len - 1] = '\0';
}

static void __del_files(const std::string &forder_path) {

	TXCPath path(forder_path);
	if (!path.is_directory()) {
		return;
	}

	TXCPathIterator iter(path);
	while (iter.valid()) {
		if (iter.path().is_file()) {
			iter.path().remove_file();
		}
		iter = iter.next();
	}
}

static void __del_timeout_file(const std::string &log_path) {
	time_t now_time = time(NULL);

	TXCPath path(log_path);

	if (path.exists() && path.is_directory()) {
		TXCPathIterator iter(path);
		while (iter.valid()) {
			time_t fileModifyTime = iter.path().last_modified_time();

			if (now_time > fileModifyTime && now_time - fileModifyTime > kMaxLogAliveTime) {
				if (iter.path().is_file()) {
					iter.path().remove_file();
				} else if (iter.path().is_directory()) {
					__del_files(iter.path().str());
				}
			}
			iter = iter.next();
		}
	}
}

static bool __append_file(const std::string &src_file, const std::string &dst_file) {
	if (src_file == dst_file) {
		return false;
	}

	TXCPath path(src_file);
	if (!path.exists()) {
		return false;
	}

	if (0 == path.file_size()) {
		return true;
	}

	FILE *srcFile = fopen(src_file.c_str(), "rb");

	if (NULL == srcFile) {
		return false;
	}

	FILE *dest_file = fopen(dst_file.c_str(), "ab");

	if (NULL == dest_file) {
		fclose(srcFile);
		return false;
	}

	fseek(srcFile, 0, SEEK_END);
	long src_file_len = ftell(srcFile);
	long dst_file_len = ftell(dest_file);
	fseek(srcFile, 0, SEEK_SET);

	char buffer[4096] = {0};

	while (true) {
		if (feof(srcFile)) break;

		size_t read_ret = fread(buffer, 1, sizeof(buffer), srcFile);

		if (read_ret == 0) break;

		if (ferror(srcFile)) break;

		fwrite(buffer, 1, read_ret, dest_file);

		if (ferror(dest_file)) break;
	}

	if (dst_file_len + src_file_len > ftell(dest_file)) {
		ftruncate(fileno(dest_file), dst_file_len);
		fclose(srcFile);
		fclose(dest_file);
		return false;
	}

	fclose(srcFile);
	fclose(dest_file);

	return true;
}

static void __move_old_files(const std::string &src_path, const std::string &dest_path, const std::string &nameprefix) {
	if (src_path == dest_path) {
		return;
	}

	TXCPath path(src_path);
	if (!path.is_directory()) {
		return;
	}

	TXCScopedLock lock_file(txv_mutex_log_file);

	TXCPathIterator iter(path);
	while (iter.valid()) {
		if (!str_util_a::starts_with(iter.path().str(), nameprefix) ||
			!str_util_a::ends_with(iter.path().str(), LOG_EXT)) {
			continue;
		}

		std::string des_file_name = dest_path + "/" + iter.path().filename();

		if (!__append_file(iter.path().str(), des_file_name)) {
			break;
		}

		iter.path().remove_file();
		iter = iter.next();
	}
}

static void __writetips2console(const char *tips_format, ...) {

	if (NULL == tips_format) {
		return;
	}

	TXSLogInfo info;
	memset(&info, 0, sizeof(TXSLogInfo));

	char tips_info[4096] = {0};
	va_list ap;
	va_start(ap, tips_format);
	vsnprintf(tips_info, sizeof(tips_info), tips_format, ap);
	va_end(ap);
    gettimeofday(&info.timeval, NULL);
	txf_console_log(&info, tips_info);
}

static bool __writefile(const void *data, size_t len, FILE *file) {
	if (NULL == file) {
		assert(false);
		return false;
	}

	long before_len = ftell(file);
	if (before_len < 0) return false;

	if (1 != fwrite(data, len, 1, file)) {
		int err = ferror(file);

		__writetips2console("write file error:%d", err);


		ftruncate(fileno(file), before_len);
		fseek(file, 0, SEEK_END);

		char err_log[256] = {0};
		snprintf(err_log, sizeof(err_log), "\nwrite file error:%d\n", err);

		char tmp[256] = {0};
		size_t nLen = sizeof(tmp);
		TXCLogBuffer::Write(err_log, strnlen(err_log, sizeof(err_log)), tmp, nLen);

		fwrite(tmp, nLen, 1, file);

		return false;
	}

	return true;
}

static bool __openlogfile(const std::string &log_dir) {
	if (txv_logdir.empty()) return false;

	struct timeval tv;
	gettimeofday(&tv, NULL);

	if (NULL != txv_logfile) {
		time_t sec = tv.tv_sec;
		tm tcur = *localtime((const time_t *) &sec);
		tm filetm = *localtime(&txv_openfiletime);

		if (filetm.tm_year == tcur.tm_year && filetm.tm_mon == tcur.tm_mon && filetm.tm_mday == tcur.tm_mday &&
			txv_current_dir == log_dir)
			return true;

		fclose(txv_logfile);
		txv_logfile = NULL;
	}

	static time_t s_last_time = 0;
	static uint64_t s_last_tick = 0;
	static char s_last_file_path[1024] = {0};

	uint64_t now_tick = txf_gettickcount();
	time_t now_time = tv.tv_sec;

	txv_openfiletime = tv.tv_sec;
	txv_current_dir = log_dir;

	char logfilepath[1024] = {0};
	__make_logfilename(tv, log_dir, txv_logfileprefix.c_str(), LOG_EXT, logfilepath, 1024);

	if (now_time < s_last_time) {
		txv_logfile = fopen(s_last_file_path, "ab");

		if (NULL == txv_logfile) {
			__writetips2console("open file error:%d %s, path:%s", errno, strerror(errno), s_last_file_path);
		}

#ifdef __APPLE__
		assert(txv_logfile);
#endif
		return NULL != txv_logfile;
	}

	txv_logfile = fopen(logfilepath, "ab");

	if (NULL == txv_logfile) {
		__writetips2console("open file error:%d %s, path:%s", errno, strerror(errno), logfilepath);
	}


	if (0 != s_last_time && (now_time - s_last_time) > (time_t) ((now_tick - s_last_tick) / 1000 + 300)) {

		struct tm tm_tmp = *localtime((const time_t *) &s_last_time);
		char last_time_str[64] = {0};
		strftime(last_time_str, sizeof(last_time_str), "%Y-%m-%d %z %H:%M:%S", &tm_tmp);

		tm_tmp = *localtime((const time_t *) &now_time);
		char now_time_str[64] = {0};
		strftime(now_time_str, sizeof(now_time_str), "%Y-%m-%d %z %H:%M:%S", &tm_tmp);

		char log[1024] = {0};
		snprintf(log, sizeof(log), "[F][ last log file:%s from %s to %s, time_diff:%ld, tick_diff:%" PRIu64 "\n",
				 s_last_file_path, last_time_str, now_time_str, now_time - s_last_time, now_tick - s_last_tick);
		char tmp[2 * 1024] = {0};
		size_t len = sizeof(tmp);
		TXCLogBuffer::Write(log, strnlen(log, sizeof(log)), tmp, len);
		__writefile(tmp, len, txv_logfile);
	}

	memcpy(s_last_file_path, logfilepath, sizeof(s_last_file_path));
	s_last_tick = now_tick;
	s_last_time = now_time;

#ifdef __APPLE__
	assert(txv_logfile);
#endif
	return NULL != txv_logfile;
}

static void __closelogfile() {
	if (NULL == txv_logfile) return;

	txv_openfiletime = 0;
	fclose(txv_logfile);
	txv_logfile = NULL;
}

static void __log2file(const void *data, size_t len) {
	if (NULL == data || 0 == len || txv_logdir.empty()) {
		return;
	}

	TXCScopedLock lock_file(txv_mutex_log_file);

	if (txv_cache_logdir.empty()) {
		if (__openlogfile(txv_logdir)) {
			__writefile(data, len, txv_logfile);
			if (kAppednerAsync == txv_mode) {
				__closelogfile();
			}
		}
		return;
	}

	struct timeval tv;
	gettimeofday(&tv, NULL);
	char logcachefilepath[1024] = {0};

	__make_logfilename(tv, txv_cache_logdir, txv_logfileprefix.c_str(), LOG_EXT, logcachefilepath, 1024);

	TXCPath path(logcachefilepath);
	if (path.exists() && __openlogfile(txv_cache_logdir)) {
		__writefile(data, len, txv_logfile);
		if (kAppednerAsync == txv_mode) {
			__closelogfile();
		}


		char logfilepath[1024] = {0};
		__make_logfilename(tv, txv_logdir, txv_logfileprefix.c_str(), LOG_EXT, logfilepath, 1024);
		if (__append_file(logcachefilepath, logfilepath)) {
			if (kAppednerSync == txv_mode) {
				__closelogfile();
			}
			remove(logcachefilepath);
		}
	} else {
		bool write_sucess = false;
		bool open_success = __openlogfile(txv_logdir);
		if (open_success) {
			write_sucess = __writefile(data, len, txv_logfile);
			if (kAppednerAsync == txv_mode) {
				__closelogfile();
			}
		}

		if (!write_sucess) {
			if (open_success && kAppednerSync == txv_mode) {
				__closelogfile();
			}

			if (__openlogfile(txv_cache_logdir)) {
				__writefile(data, len, txv_logfile);
				if (kAppednerAsync == txv_mode) {
					__closelogfile();
				}
			}
		}
	}

}


static void __writetips2file(const char *tips_format, ...) {

	if (NULL == tips_format) {
		return;
	}

	char tips_info[4096] = {0};
	va_list ap;
	va_start(ap, tips_format);
	vsnprintf(tips_info, sizeof(tips_info), tips_format, ap);
	va_end(ap);

	char tmp[8 * 1024] = {0};
	size_t len = sizeof(tmp);

	TXCLogBuffer::Write(tips_info, strnlen(tips_info, sizeof(tips_info)), tmp, len);

	__log2file(tmp, len);
}

static void __async_log_thread() {
	while (true) {

		TXCScopedLock lock_buffer(txv_mutex_buffer_async);

		if (NULL == txv_log_buff) break;

		TXCAutoBuffer tmp;
		txv_log_buff->Flush(tmp);
		lock_buffer.unlock();

		if (NULL != tmp.Ptr()) __log2file(tmp.Ptr(), tmp.Length());

		if (txv_log_close) break;

		txv_cond_buffer_async.wait(15 * 60 * 1000);
	}
}

static void __appender_sync(const TXSLogInfo *info, const char *log) {

	char temp[16 * 1024] = {0};     // tell perry,ray if you want modify size.
	TXCPtrBuffer log_buff(temp, 0, sizeof(temp));
	txf_log_formater(info, log, log_buff);

	char buffer_crypt[16 * 1024] = {0};
	size_t len = 16 * 1024;
	if (!TXCLogBuffer::Write(log_buff.Ptr(), log_buff.Length(), buffer_crypt, len)) return;

	__log2file(buffer_crypt, len);
}

static void __appender_async(const TXSLogInfo *info, const char *log) {
	TXCScopedLock lock(txv_mutex_buffer_async);
	if (NULL == txv_log_buff) return;

	char temp[16 * 1024] = {0};       //tell perry,ray if you want modify size.
	TXCPtrBuffer log_buff(temp, 0, sizeof(temp));
	txf_log_formater(info, log, log_buff);

	if (txv_log_buff->GetData().Length() >= kBufferBlockLength * 4 / 5) {
		int ret = snprintf(temp, sizeof(temp), "[F][ sg_buffer_async.Length() >= BUFFER_BLOCK_LENTH*4/5, len: %d\n",
						   (int) txv_log_buff->GetData().Length());
		log_buff.Length(ret, ret);
	}

	if (!txv_log_buff->Write(log_buff.Ptr(), (unsigned int) log_buff.Length())) return;

	if (txv_log_buff->GetData().Length() >= kBufferBlockLength * 1 / 3 ||
		(NULL != info && kLevelFatal == info->level)) {
		txv_cond_buffer_async.notifyAll();
	}

}

////////////////////////////////////////////////////////////////////////////////////

void txclogger_appender(const TXSLogInfo *info, const char *log) {
	if (txv_log_close) return;

	SCOPE_ERRNO();

	DEFINE_TXCSCOPERECURSIONLIMIT(recursion);
	static TXCThreadLocalStore s_recursion_str(free);

	if (txv_consolelog_open) txf_console_log(info, log);

	if (2 <= recursion.Get() && NULL == s_recursion_str.get()) {
		if (recursion.Get() > 10) return;
		char *strrecursion = (char *) calloc(16 * 1024, 1);
		s_recursion_str.set((void*)(strrecursion));

		TXSLogInfo nInfo = *info;
		nInfo.level = kLevelFatal;

		char recursive_log[256] = {0};
		snprintf(recursive_log, sizeof(recursive_log), "ERROR!!! txclogger_appender Recursive calls!!!, count:%d",
				(int)recursion.Get());

		TXCPtrBuffer tmp(strrecursion, 0, 16 * 1024);
		txf_log_formater(&nInfo, recursive_log, tmp);

		strncat(strrecursion, log, 4096);
		strrecursion[4095] = '\0';

		txf_console_log(&nInfo, strrecursion);
	} else {
		if (NULL != s_recursion_str.get()) {
			char *strrecursion = (char*)s_recursion_str.get();
			s_recursion_str.set(NULL);

			__writetips2file(strrecursion);
			free(strrecursion);
		}

		if (kAppednerSync == txv_mode)
			__appender_sync(info, log);
		else
			__appender_async(info, log);
	}
}

#define HEX_STRING  "0123456789abcdef"

static unsigned int to_string(const void *signature, int len, char *str) {
	char *str_p = str;
	const unsigned char *sig_p;

	for (sig_p = (const unsigned char *) signature; sig_p - (const unsigned char *) signature < len; sig_p++) {
		char high, low;
		high = *sig_p / 16;
		low = *sig_p % 16;

		*str_p++ = HEX_STRING[(unsigned char) high];
		*str_p++ = HEX_STRING[(unsigned char) low];
		*str_p++ = ' ';
	}

	*str_p++ = '\n';

	for (sig_p = (const unsigned char *) signature; sig_p - (const unsigned char *) signature < len; sig_p++) {
		*str_p++ = char(isgraph(*sig_p) ? *sig_p : ' ');
		*str_p++ = ' ';
		*str_p++ = ' ';
	}

	return (unsigned int) (str_p - str);
}

const char *txf_logger_dump(const void *dumpbuffer, size_t len) {
	if (NULL == dumpbuffer || 0 == len) {
		//        ASSERT(NULL!=dumpbuffer);
		//        ASSERT(0!=len);
		return "";
	}

	SCOPE_ERRNO();

	if (NULL == txv_tss_dumpfile.get()) {
		txv_tss_dumpfile.set(calloc(4096, 1));
	} else {
		memset(txv_tss_dumpfile.get(), 0, 4096);
	}

	ASSERT(NULL != txv_tss_dumpfile.get());

	struct timeval tv = {0};
	gettimeofday(&tv, NULL);
	time_t sec = tv.tv_sec;
	tm tcur = *localtime((const time_t *) &sec);

	char forder_name[128] = {0};
	snprintf(forder_name, sizeof(forder_name), "%d%02d%02d", 1900 + tcur.tm_year, 1 + tcur.tm_mon, tcur.tm_mday);

	std::string filepath = txv_logdir + "/" + forder_name + "/";

	TXCPath path(filepath);
	if (!path.exists())
		path.create_directory();


	char file_name[128] = {0};
	snprintf(file_name, sizeof(file_name), "%d%02d%02d%02d%02d%02d_%d.dump", 1900 + tcur.tm_year, 1 + tcur.tm_mon,
			 tcur.tm_mday,
			 tcur.tm_hour, tcur.tm_min, tcur.tm_sec, (int) len);
	filepath += file_name;

	FILE *fileid = fopen(filepath.c_str(), "wb");

	if (NULL == fileid) {
		ASSERT2(NULL != fileid, "%s, errno:(%d, %s)", filepath.c_str(), errno, strerror(errno));
		return "";
	}

	fwrite(dumpbuffer, len, 1, fileid);
	fclose(fileid);

	char *dump_log = (char *) txv_tss_dumpfile.get();
	dump_log += snprintf(dump_log, 4096, "\n dump file to %s :\n", filepath.c_str());

	int dump_len = 0;

	for (int x = 0; x < 32 && dump_len < (int) len; ++x) {
		dump_log += to_string((const char *) dumpbuffer + dump_len, std::min(int(len) - dump_len, 16), dump_log);
		dump_len += std::min((int) len - dump_len, 16);
		*(dump_log++) = '\n';
	}

	return (const char *) txv_tss_dumpfile.get();
}


static void get_mark_info(char *_info, size_t _infoLen) {
	struct timeval tv;
	gettimeofday(&tv, 0);
	time_t sec = tv.tv_sec;
	struct tm tm_tmp = *localtime((const time_t *) &sec);
	char tmp_time[64] = {0};
	strftime(tmp_time, sizeof(tmp_time), "%Y-%m-%d %z %H:%M:%S", &tm_tmp);
	snprintf(_info, _infoLen, "[%" PRIdMAX ",%" PRIdMAX "][%s]", txf_logger_pid(), txf_logger_tid(), tmp_time);
}

void txf_appender_open(TXEAppenderMode mode, const char *dir, const char *nameprefix) {
	assert(dir);
	assert(nameprefix);

	if (!txv_log_close) {
		__writetips2file("appender has already been opened. dir:%s nameprefix:%s", dir, nameprefix);
		return;
	}

	txf_logger_set_appender(&txclogger_appender);

	//mkdir(dir, S_IRWXU|S_IRWXG|S_IRWXO);
	TXCPath path(dir);
	if (!path.create_directory()) {
		__writetips2console("create directory error:%d %s, path:%s", errno, strerror(errno), dir);
	}
	TXCTickCount tick;
	tick.getTickCount();
	__del_timeout_file(dir);

	TXCTickCountDiff del_timeout_file_time = TXCTickCount().getTickCount() - tick;

	tick.getTickCount();

	char mmap_file_path[512] = {0};
	snprintf(mmap_file_path, sizeof(mmap_file_path), "%s/%s.mmap2",
			 txv_cache_logdir.empty() ? dir : txv_cache_logdir.c_str(), nameprefix);

	bool use_mmap = false;
	if (txf_open_mmap_file(mmap_file_path, kBufferBlockLength, txv_mmap_file)) {
		txv_log_buff = new TXCLogBuffer(txv_mmap_file.data(), kBufferBlockLength, true);
		use_mmap = true;
	} else {
		char *buffer = new char[kBufferBlockLength];
		txv_log_buff = new TXCLogBuffer(buffer, kBufferBlockLength, true);
		use_mmap = false;
	}

	if (NULL == txv_log_buff->GetData().Ptr()) {
		if (use_mmap && txv_mmap_file.is_open()) txf_close_mmap_file(txv_mmap_file);
		return;
	}


	TXCAutoBuffer buffer;
	txv_log_buff->Flush(buffer);

	TXCScopedLock lock(txv_mutex_log_file);
	txv_logdir = dir;
	txv_logfileprefix = nameprefix;
	txv_log_close = false;
	txf_appender_setmode(mode);
	lock.unlock();

	char mark_info[512] = {0};
	get_mark_info(mark_info, sizeof(mark_info));

	if (buffer.Ptr()) {
		__writetips2file("~~~~~ begin of mmap ~~~~~\n");
		__log2file(buffer.Ptr(), buffer.Length());
		__writetips2file("~~~~~ end of mmap ~~~~~%s\n", mark_info);
	}

	TXCTickCountDiff get_mmap_time = TXCTickCount().getTickCount() - tick;


	char appender_info[728] = {0};
	snprintf(appender_info, sizeof(appender_info), "^^^^^^^^^^" __DATE__ "^^^" __TIME__ "^^^^^^^^^^%s", mark_info);

	txclogger_appender(NULL, appender_info);
	char logmsg[64] = {0};
	snprintf(logmsg, sizeof(logmsg), "del time out files time: %" PRIu64, (int64_t) del_timeout_file_time);
	txclogger_appender(NULL, logmsg);

	snprintf(logmsg, sizeof(logmsg), "get mmap time: %" PRIu64, (int64_t) get_mmap_time);
	txclogger_appender(NULL, logmsg);

	snprintf(logmsg, sizeof(logmsg), "SDK_VERSION: %s", txf_get_sdk_version());
	txclogger_appender(NULL, logmsg);

	snprintf(logmsg, sizeof(logmsg), "SDK_ID: %d", txf_get_sdk_id());
	txclogger_appender(NULL, logmsg);

	snprintf(logmsg, sizeof(logmsg), "log appender mode:%d, use mmap:%d", (int) mode, use_mmap);
	txclogger_appender(NULL, logmsg);

	BOOT_RUN_EXIT(txf_appender_close);

}

void txf_appender_open_with_cache(TXEAppenderMode mode, const std::string &cachedir, const std::string &logdir,
								  const char *nameprefix) {
	assert(!cachedir.empty());
	assert(!logdir.empty());
	assert(nameprefix);

	txv_logdir = logdir;

	if (!cachedir.empty()) {
		txv_cache_logdir = cachedir;
		TXCPath path(cachedir);
		path.create_directory();
		__del_timeout_file(cachedir);
		// "nameprefix" must explicitly convert to "std::string", or when the thread is ready to run, "nameprefix" has been released.
		TXCThread(std::bind(&__move_old_files, cachedir, logdir, std::string(nameprefix))).start_after(3 * 60 * 1000);
	}

	txf_appender_open(mode, logdir.c_str(), nameprefix);

}

void txf_appender_flush() {
	txv_cond_buffer_async.notifyAll();
}

void txf_appender_flush_sync() {
	if (kAppednerSync == txv_mode) {
		return;
	}

	TXCScopedLock lock_buffer(txv_mutex_buffer_async);

	if (NULL == txv_log_buff) return;

	TXCAutoBuffer tmp;
	txv_log_buff->Flush(tmp);

	lock_buffer.unlock();

	if (tmp.Ptr()) __log2file(tmp.Ptr(), tmp.Length());

}

void txf_appender_close() {
	if (txv_log_close) return;

	char mark_info[512] = {0};
	get_mark_info(mark_info, sizeof(mark_info));
	char appender_info[728] = {0};
	snprintf(appender_info, sizeof(appender_info), "$$$$$$$$$$" __DATE__ "$$$" __TIME__ "$$$$$$$$$$%s\n", mark_info);
	txclogger_appender(NULL, appender_info);

	txv_log_close = true;

	txv_cond_buffer_async.notifyAll();

	if (txv_thread_async.isruning())
		txv_thread_async.join();


	TXCScopedLock buffer_lock(txv_mutex_buffer_async);
	if (txv_mmap_file.is_open()) {
		memset(txv_mmap_file.data(), 0, kBufferBlockLength);

		txf_close_mmap_file(txv_mmap_file);
	} else {
		delete[] (char *) ((txv_log_buff->GetData()).Ptr());
	}

	delete txv_log_buff;
	txv_log_buff = NULL;
	buffer_lock.unlock();

	TXCScopedLock lock(txv_mutex_log_file);
	__closelogfile();
}

void txf_appender_setmode(TXEAppenderMode mode) {
	txv_mode = mode;

	txv_cond_buffer_async.notifyAll();

	if (kAppednerAsync == txv_mode && !txv_thread_async.isruning()) {
		txv_thread_async.start();
	}
}

bool txf_appender_get_current_log_path(char *log_path, unsigned int len) {
	if (NULL == log_path || 0 == len) return false;

	if (txv_logdir.empty()) return false;

	strncpy(log_path, txv_logdir.c_str(), len - 1);
	log_path[len - 1] = '\0';
	return true;
}

bool txf_appender_get_current_log_cache_path(char *logPath, unsigned int len) {
	if (NULL == logPath || 0 == len) return false;

	if (txv_cache_logdir.empty()) return false;
	strncpy(logPath, txv_cache_logdir.c_str(), len - 1);
	logPath[len - 1] = '\0';
	return true;
}

void txf_appender_set_console_log(bool is_open) {
	txv_consolelog_open = is_open;
}

void appender_setExtraMSg(const char *msg, unsigned int len) {
	txv_log_extra_msg = std::string(msg, len);
}

bool txf_appender_getfilepath_from_timespan(int timespan, const char *prefix, std::vector<std::string> &filepath_vec) {
	if (txv_logdir.empty()) return false;

	struct timeval tv;
	gettimeofday(&tv, NULL);
	tv.tv_sec -= timespan * (24 * 60 * 60);

	char log_path[2048] = {0};
	__make_logfilename(tv, txv_logdir, prefix, LOG_EXT, log_path, sizeof(log_path));

	filepath_vec.push_back(log_path);

	if (txv_cache_logdir.empty()) {
		return true;
	}

	memset(log_path, 0, sizeof(log_path));
	__make_logfilename(tv, txv_cache_logdir, prefix, LOG_EXT, log_path, sizeof(log_path));

	filepath_vec.push_back(log_path);

	return true;
}
