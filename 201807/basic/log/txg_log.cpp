//
//  txg_log.cpp
//  log
//
//  Created by alderzhang on 2017/5/17.
//  Copyright © 2017年 Tencent. All rights reserved.
//
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include "txg_log.h"

txf_log_hooker g_logObserver = 0;

void txf_log_set_hooker(txf_log_hooker observer) {
    g_logObserver = observer;
}

#ifdef ENABLE_LOG_IMPL

#include "txg_appender.h"
#include "TXCLogger.h"

extern "C" {
    
void txf_log(TXELogLevel level, const char *file, int line, const char *func, const char *format, ...){
    if (!txf_logger_is_enabled_for((TXELoggerLevel) level)) return;
    char strBuf[16 * 1024];
    va_list pArgs;
    va_start(pArgs, format);
    vsprintf(strBuf, format, pArgs);
    va_end(pArgs);
    
    if (g_logObserver) {
        g_logObserver(level, file, strBuf);
//        return;
    }
    
    TXSLogInfo info = {(TXELoggerLevel) level, "", file, func, line, {0, 0}, -1, -1, -1};
    gettimeofday(&info.timeval, NULL);
    txf_logger_write(&info, strBuf);
}

void txf_log_set_level(TXELogLevel level) {
    txf_logger_set_level((TXELoggerLevel) level);
}

void txf_log_set_console(bool is_open) {
    txf_appender_set_console_log(is_open);
}

void txf_log_open(TXELogMode mode, const char *dir, const char *nameprefix) {
    txf_appender_open((enum TXEAppenderMode) mode, dir, nameprefix);
}
    
void txf_log_flush(){
    txf_appender_flush_sync();
}

void txf_log_close() {
    txf_appender_close();
}

}

#else

extern "C"{
    
void txf_log(TXELogLevel level, const char *file, int line, const char *func, const char *format, ...){
    char strBuf[16 * 1024];
    va_list pArgs;
    va_start(pArgs, format);
    vsprintf(strBuf, format, pArgs);
    va_end(pArgs);
    
    if (g_logObserver) {
        g_logObserver(level, file, strBuf);
        return;
    }
}

void txf_log_set_level(TXELogLevel level){
    
}

void txf_log_set_console(bool is_open){
    
}

void txf_log_open(TXELogMode mode, const char *dir, const char *nameprefix){
    
}
    
void txf_log_flush(){

}

void txf_log_close(){
    
}
    
}

#endif
