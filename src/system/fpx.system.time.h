#ifndef FPX_SYSTEM_TIME_H
#define FPX_SYSTEM_TIME_H

#include "fpx.system.os.portable.h"

#if (FPX_POSIX)

#define fpx_tm_sec     tm_sec
#define fpx_tm_min     tm_min
#define fpx_tm_hour    tm_hour
#define fpx_tm_mday    tm_mday
#define fpx_tm_mon     tm_mon
#define fpx_tm_year    tm_year
#define fpx_tm_wday    tm_wday

#define fpx_tm_sec_t   int
#define fpx_tm_min_t   int
#define fpx_tm_hour_t  int
#define fpx_tm_mday_t  int
#define fpx_tm_mon_t   int
#define fpx_tm_year_t  int
#define fpx_tm_wday_t  int

#define fpx_msleep(ms) usleep(ms * 1000)
#define fpx_pause()    pause()

#elif (FPX_WIN32)

#define fpx_tm_sec     wSecond
#define fpx_tm_min     wMinute
#define fpx_tm_hour    wHour
#define fpx_tm_mday    wDay
#define fpx_tm_mon     wMonth
#define fpx_tm_year    wYear
#define fpx_tm_wday    wDayOfWeek

#define fpx_tm_sec_t   WORD
#define fpx_tm_min_t   WORD
#define fpx_tm_hour_t  WORD
#define fpx_tm_mday_t  WORD
#define fpx_tm_mon_t   WORD
#define fpx_tm_year_t  WORD
#define fpx_tm_wday_t  WORD

#define fpx_msleep(ms) Sleep(ms)

#endif

typedef uintptr_t fpx_msec_t;

void fpx_gettimeofday(struct timeval *tv);
void fpx_localtime(time_t sec, fpx_tm_t *tm);

#endif /* FPX_SYSTEM_TIME_H */
