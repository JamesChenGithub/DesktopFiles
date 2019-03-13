/*
 ============================================================================
 Name        : txcloggerbase.c
 ============================================================================
 */

#include "txg_log_base.h"
#include <stdio.h>

#include "txg_compiler_util.h"

WEAK_FUNC  TXELoggerLevel   __txclogger_Level_impl();
WEAK_FUNC  void        __txclogger_SetLevel_impl(TXELoggerLevel _level);
WEAK_FUNC  int         __txclogger_IsEnabledFor_impl(TXELoggerLevel _level);
WEAK_FUNC txf_logger_appender_t __txclogger_SetAppender_impl(txf_logger_appender_t _appender);
WEAK_FUNC void __txf_logger_write_impl(const TXSLogInfo* _info, const char* _log);
WEAK_FUNC void __txf_logger_vprint_impl(const TXSLogInfo* _info, const char* _format, va_list _list);

WEAK_FUNC void __txf_logger_assert_p_impl(const TXSLogInfo* _info, const char* _expression, const char* _format, va_list _list);
WEAK_FUNC void __txf_logger_assert_impl(const TXSLogInfo* _info, const char* _expression, const char* _log);

#ifndef WIN32
WEAK_FUNC const char* txf_logger_dump(const void *_dumpbuffer, size_t _len) { return "";}
#endif

TXELoggerLevel   txf_logger_level() {
    if (NULL == &__txclogger_Level_impl)  return kLevelNone;
	return __txclogger_Level_impl();
}

void txf_logger_set_level(TXELoggerLevel level){
    if (NULL != &__txclogger_SetLevel_impl)
        __txclogger_SetLevel_impl(level);
}

int  txf_logger_is_enabled_for(TXELoggerLevel level) {
    if (NULL == &__txclogger_IsEnabledFor_impl) { return 0;}
	return __txclogger_IsEnabledFor_impl(level);

}

txf_logger_appender_t txf_logger_set_appender(txf_logger_appender_t appender) {
    if (NULL == &__txclogger_SetAppender_impl) { return NULL;}
    return __txclogger_SetAppender_impl(appender);
}

void txf_logger_write(const TXSLogInfo *info, const char *log) {
	if (NULL != &__txf_logger_write_impl)
		__txf_logger_write_impl(info, log);
}

void txf_logger_vprint(const TXSLogInfo *info, const char *format, va_list list) {
	if (NULL != &__txf_logger_vprint_impl)
		__txf_logger_vprint_impl(info, format, list);
}

void txf_logger_print(const TXSLogInfo *info, const char *format, ...) {
	if (NULL == &__txf_logger_vprint_impl){ return; }
    
	va_list valist;
	va_start(valist, format);
    __txf_logger_vprint_impl(info, format, valist);
	va_end(valist);
}


void txf_logger_assert_p(const TXSLogInfo *info, const char *expression, const char *format, ...) {
	if (NULL == &__txf_logger_assert_p_impl)  { return; }
    
	va_list valist;
	va_start(valist, format);
	__txf_logger_assert_p_impl(info, expression, format, valist);
	va_end(valist);
}

void txf_logger_assert(const TXSLogInfo *info, const char *expression, const char *log) {
    if (NULL != &__txf_logger_assert_impl)
    	__txf_logger_assert_impl(info, expression, log);
}


#ifndef USING_XLOG_WEAK_FUNC
static TXELoggerLevel gs_level = kLevelNone;
static txf_logger_appender_t gs_appender = NULL;

TXELoggerLevel   __txclogger_Level_impl() {return gs_level;}
void        __txclogger_SetLevel_impl(TXELoggerLevel _level){ gs_level = _level;}
int         __txclogger_IsEnabledFor_impl(TXELoggerLevel _level) {return gs_level <= _level;}

txf_logger_appender_t __txclogger_SetAppender_impl(txf_logger_appender_t _appender)  {
    txf_logger_appender_t old_appender = gs_appender;
    gs_appender = _appender;
    return old_appender;
}

void __txf_logger_write_impl(const TXSLogInfo* _info, const char* _log) {
    
    if (!gs_appender) return;
    
    if (_info && -1==_info->pid && -1==_info->tid && -1==_info->maintid)
    {
        TXSLogInfo* info = (TXSLogInfo*)_info;
        info->pid = txf_logger_pid();
        info->tid = txf_logger_tid();
        info->maintid = txf_logger_main_tid();
    }
    
    if (NULL == _log) {
        if (_info) {
            TXSLogInfo* info = (TXSLogInfo*)_info;
            info->level = kLevelFatal;
        }
        gs_appender(_info, "NULL == _log");
    } else {
        gs_appender(_info, _log);
    }
}

void __txf_logger_vprint_impl(const TXSLogInfo* _info, const char* _format, va_list _list) {
    if (NULL == _format) {
        TXSLogInfo* info = (TXSLogInfo*)_info;
        info->level = kLevelFatal;
        __txf_logger_write_impl(_info, "NULL == _format");
    } else {
        char temp[4096] = {'\0'};
        vsnprintf(temp, 4096, _format, _list);
        __txf_logger_write_impl(_info, temp);
    }
}

extern void txf_assertv2(const char *_pfile, int _line, const char *_pfunc, const char *_pexpression,
						 const char *_format, va_list _list);
void __txf_logger_assert_p_impl(const TXSLogInfo* _info, const char* _expression, const char* _format, va_list _list) {
	txf_assertv2(_info->filename, _info->line, _info->func_name, _expression, _format, _list);
}

extern void txf_assert2(const char *_pfile, int _line, const char *_pfunc, const char *_pexpression,
						const char *_format, ...);
void __txf_logger_assert_impl(const TXSLogInfo* _info, const char* _expression, const char* _log) {
	txf_assert2(_info->filename, _info->line, _info->func_name, _expression, _log);
    
}
#endif
