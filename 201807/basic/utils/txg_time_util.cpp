/*
 * utils.c
 *
 *  Created on: 2012-7-18
 *      Author: yerungui
 */

#include "txg_time_util.h"
#include <chrono>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/syscall.h>
#if defined(__MACH__)
#include <mach/mach_time.h>
#endif

extern "C"{

uint64_t txf_gettickcount() {
    unsigned long long tickGet = 0;
    
#if defined(__MACH__)//for ios and macos
    return (uint64_t) std::chrono::steady_clock::now().time_since_epoch().count() / 1000000;
#else
    struct timespec curr_time = {0, 0};
    
    syscall(__NR_clock_gettime, CLOCK_MONOTONIC_RAW, &curr_time);
    
    tickGet = ((unsigned long long)curr_time.tv_sec)*1000 + curr_time.tv_nsec/1000/1000;
#endif
    return tickGet;
}

uint64_t txf_getutctick(){
    return (uint64_t) std::chrono::system_clock::now().time_since_epoch().count() / 1000;
}

uint64_t txf_gettickspan(uint64_t lastTick){
    return txf_gettickcount() - lastTick;
}

}
