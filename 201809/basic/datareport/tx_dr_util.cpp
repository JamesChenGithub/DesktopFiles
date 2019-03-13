#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/syscall.h>
#if defined(__MACH__)
#include <mach/mach_time.h>
#endif
#include "tx_dr_util.h"

unsigned int tx_rtmp_msleep(unsigned int milliseconds) {
    struct timespec 	tswait;   	/* Input for timedwait */
    /* Calculate the end time*/
    tswait.tv_sec = milliseconds/1000;
    tswait.tv_nsec = (milliseconds % 1000) * 1000 * 1000;
    /* High resolution to sleep */
    return nanosleep(&tswait, NULL);
}
