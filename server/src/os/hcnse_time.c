#include "hcnse_portable.h"
#include "hcnse_core.h"


// static char *week[] = {
//     "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
// };

// static char *months[] = {
//     "Jan", "Feb", "Mar", "Apr", "May", "Jun",
//     "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
// };


#if (HCNSE_POSIX)

void
hcnse_gettimeofday(struct timeval *tv)
{
    gettimeofday(tv, NULL);
}

void
hcnse_localtime(time_t sec, hcnse_tm_t *tm)
{
    localtime_r(&sec, tm);
    tm->tm_mon += 1;
    tm->tm_year += 1900;
}


#elif (HCNSE_WIN32)

void
hcnse_gettimeofday(struct timeval *tv)
{

    /* 
     * It is not recommended that you add and subtract values
     * from the SYSTEMTIME structure to obtain relative times.
     * Instead, you should:
     * - Convert the SYSTEMTIME structure to a FILETIME structure;
     * - Copy the resulting FILETIME structure to a ULARGE_INTEGER structure;
     * - Use normal 64-bit arithmetic on the ULARGE_INTEGER value.
     */

    uint64_t intervals;
    FILETIME ft;

    GetSystemTimeAsFileTime(&ft);

    intervals = ((uint64_t) ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    intervals -= 116444736000000000;

    tv->tv_sec = (long) (intervals / 10000000);
    tv->tv_usec = (long) ((intervals % 10000000) / 10);
}

void
hcnse_localtime(time_t sec, hcnse_tm_t *tm)
{
    (void) sec;
    GetLocalTime(tm);
}
#endif


const char *
hcnse_timestr(time_t sec, const char *fmt, char *buf, size_t len)
{
    hcnse_tm_t tm;
    size_t n;

    hcnse_localtime(sec, &tm);

    n = hcnse_snprintf(buf, len, fmt,
        tm.hcnse_tm_mday, tm.hcnse_tm_mon, tm.hcnse_tm_year,
        tm.hcnse_tm_hour, tm.hcnse_tm_min, tm.hcnse_tm_sec);

    buf[n] = '\0';

    return buf;
}
