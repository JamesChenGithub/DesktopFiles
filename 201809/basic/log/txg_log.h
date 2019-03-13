//
//  txg_log.h
//  log
//
//  Created by alderzhang on 2017/5/17.
//  Copyright © 2017年 Tencent. All rights reserved.
//

#ifndef txg_log_h
#define txg_log_h

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

typedef enum {
    TXE_LOG_VERBOSE = 0,
    TXE_LOG_DEBUG,
    TXE_LOG_INFO,
    TXE_LOG_WARNING,
    TXE_LOG_ERROR,
    TXE_LOG_FATAL,
    TXE_LOG_NONE,
} TXELogLevel;

typedef enum {
    TXE_LOG_ASYNC = 0,
    TXE_LOG_SYNC,
} TXELogMode;
    
    
typedef void (*txf_log_hooker)(TXELogLevel level, const char* file, const char* log);
    
void txf_log_set_hooker(txf_log_hooker observer);

void txf_log(TXELogLevel level, const char *file, int line, const char *func, const char *format, ...);

void txf_log_set_level(TXELogLevel level);

void txf_log_set_console(bool is_open);

void txf_log_open(TXELogMode mode, const char *dir, const char *nameprefix);
    
void txf_log_flush();

void txf_log_close();

#ifdef __cplusplus
}
#endif


#define LOGE(fmt, ...) \
    txf_log(TXE_LOG_ERROR, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) \
    txf_log(TXE_LOG_WARNING, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) \
    txf_log(TXE_LOG_INFO, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) \
    txf_log(TXE_LOG_DEBUG, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOGV(fmt, ...) \
    txf_log(TXE_LOG_VERBOSE, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

#endif /* txg_log_h */
