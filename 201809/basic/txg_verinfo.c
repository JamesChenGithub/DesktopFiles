#include "txg_verinfo.h"

/*
 * Define SDK_VERSION and SDK_ID below
 */

//#define SDK_VERSION
//#define SDK_ID

#ifndef SDK_VERSION
#define SDK_VERSION "2.1.0"
//必须自行定义宏参数SDK_VERSION，指定SDK版本号
#warning "MACRO <SDK_VERSION> must be defined"
#endif

#ifndef SDK_ID
#define SDK_ID 0
//必须自行定义宏参数SDK_ID，指定SDK类型
#warning "MACRO <SDK_ID> must be defined"
#endif

const char *txf_get_sdk_version(){
    return SDK_VERSION;
}

int txf_get_sdk_id(){
    return SDK_ID;
}
