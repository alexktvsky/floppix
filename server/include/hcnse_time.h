#ifndef INCLUDED_HCNSE_TIME_H
#define INCLUDED_HCNSE_TIME_H

#include "hcnse_portable.h"
#include "hcnse_core.h"

#define HCNSE_TIMESTRLEN               24

#if (HCNSE_POSIX)

#define hcnse_tm_sec                   tm_sec
#define hcnse_tm_min                   tm_min
#define hcnse_tm_hour                  tm_hour
#define hcnse_tm_mday                  tm_mday
#define hcnse_tm_mon                   tm_mon
#define hcnse_tm_year                  tm_year
#define hcnse_tm_wday                  tm_wday

#define hcnse_tm_sec_t                 int
#define hcnse_tm_min_t                 int
#define hcnse_tm_hour_t                int
#define hcnse_tm_mday_t                int
#define hcnse_tm_mon_t                 int
#define hcnse_tm_year_t                int
#define hcnse_tm_wday_t                int

#define hcnse_msleep(ms)               usleep(ms * 1000)
#define hcnse_pause()                  pause()


#elif (HCNSE_WIN32)

#define hcnse_tm_sec                   wSecond
#define hcnse_tm_min                   wMinute
#define hcnse_tm_hour                  wHour
#define hcnse_tm_mday                  wDay
#define hcnse_tm_mon                   wMonth
#define hcnse_tm_year                  wYear
#define hcnse_tm_wday                  wDayOfWeek

#define hcnse_tm_sec_t                 WORD
#define hcnse_tm_min_t                 WORD
#define hcnse_tm_hour_t                WORD
#define hcnse_tm_mday_t                WORD
#define hcnse_tm_mon_t                 WORD
#define hcnse_tm_year_t                WORD
#define hcnse_tm_wday_t                WORD

#define hcnse_msleep(ms)               Sleep(ms)

#endif

void hcnse_gettimeofday(struct timeval *tv);

void hcnse_localtime(time_t sec, hcnse_tm_t *tm);
const char *hcnse_timestr(char *buf, size_t len, time_t sec);

#endif /* INCLUDED_HCNSE_TIME_H */
