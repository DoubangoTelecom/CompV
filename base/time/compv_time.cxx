/* Copyright (C) 2011-2020 Doubango Telecom <https://www.doubango.org>
* File author: Mamadou DIOP (Doubango Telecom, France).
* License: GPLv3. For commercial license please contact us.
* Source code: https://github.com/DoubangoTelecom/compv
* WebSite: http://compv.org
*/
#include "compv/base/time/compv_time.h"
#include "compv/base/compv_debug.h"

#if COMPV_OS_WINDOWS
#	include <Winsock2.h> // timeval
#	include <windows.h>
#	if COMPV_OS_WINDOWS_CE && HAVE_WCECOMPAT
#		include <sys/time.h>
#	endif
#elif COMPV_OS_SYMBIAN32
#	include <_timeval.h>
#else
#	include <sys/time.h>
#endif

#include <time.h>
#if defined (COMPV_OS_APPLE)
#	include <mach/mach.h>
#	include <mach/mach_time.h>
#endif

#if !HAVE_GETTIMEOFDAY
#if COMPV_OS_WINDOWS

/* For windows implementation of "gettimeofday" Thanks to "http://www.cpp-programming.net/c-tidbits/gettimeofday-function-for-windows" */
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000ULL
#endif

struct timezone {
    int  tz_minuteswest; // minutes W of Greenwich
    int  tz_dsttime;     // type of dst correction
};

static int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    FILETIME ft;
    uint64_t tmpres = 0;
    static int tzflag = 0;

    if (tv) {
#if COMPV_OS_WINDOWS_CE
        SYSTEMTIME st;
        GetSystemTime(&st);
        SystemTimeToFileTime(&st, &ft);
#else
        GetSystemTimeAsFileTime(&ft);
#endif /* COMPV_OS_WINDOWS_CE */

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        /*converting file time to unix epoch*/
        tmpres /= 10;  /*convert into microseconds*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

#if !COMPV_OS_WINDOWS_CE
    if (tz) {
        if (!tzflag) {
#if !COMPV_OS_WINDOWS_RT
            _tzset();
#endif /* COMPV_OS_WINDOWS_RT */
            tzflag++;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
    }
#endif /* COMPV_OS_WINDOWS_CE */

    return 0;
}

#else
#pragma error "You MUST provide an implement for 'gettimeofday'"
#endif /* WIN32 */

#endif /* !HAVE_GETTIMEOFDAY */

COMPV_NAMESPACE_BEGIN()

/**
* This function shall obtain the current time, expressed as seconds and microseconds since EPOCH (00:00:00 UTC on 1 January 1970).
* The resolution of the system clock is unspecified.
* @param tv The current time, expressed as seconds and microseconds since EPOCH(00:00:00 UTC on 1 January 1970).
* @param tz The timezone.
* @retval The function shall return 0 and no value shall be reserved to indicate an error.
*/
int CompVTime::timeOfDay(struct timeval *tv, struct timezone *tz)
{
    return gettimeofday(tv, tz);
}

uint64_t CompVTime::timeOfDayMillis()
{
    struct timeval tv;
    CompVTime::timeOfDay(&tv, NULL);
    return (((uint64_t)tv.tv_sec)*(uint64_t)1000) + (((uint64_t)tv.tv_usec) / (uint64_t)1000);
}

/**
* Gets the number of milliseconds in @a tv
* @retval The number of milliseconds
*/
uint64_t CompVTime::millis(const struct timeval *tv)
{
    if (!tv) {
        COMPV_DEBUG_ERROR("Invalid parameter");
        return 0;
    }
    return (((uint64_t)tv->tv_sec)*(uint64_t)1000) + (((uint64_t)tv->tv_usec) / (uint64_t)1000);
}

/**
* Gets the number of milliseconds since the EPOCH.
* @retval The number of milliseconds since EPOCH.
*/
uint64_t CompVTime::epochMillis()
{
    struct timeval tv;
    gettimeofday(&tv, (struct timezone *)NULL);
    return (((uint64_t)tv.tv_sec)*(uint64_t)1000) + (((uint64_t)tv.tv_usec) / (uint64_t)1000);
}

// /!\ NOT CURRENT TIME
// only make sense when comparing two values (e.g. for duration)
uint64_t CompVTime::nowMillis()
{
#if COMPV_OS_WINDOWS
    static int __cpu_count = 0;
    if (__cpu_count == 0) {
        SYSTEM_INFO SystemInfo;
#	if COMPV_OS_WINDOWS_RT
        GetNativeSystemInfo(&SystemInfo);
#	else
        GetSystemInfo(&SystemInfo);
#	endif
        __cpu_count = SystemInfo.dwNumberOfProcessors;
    }
    if (__cpu_count == 1) {
        static LARGE_INTEGER __liFrequency = { 0 };
        LARGE_INTEGER liPerformanceCount;
        if (!__liFrequency.QuadPart) {
            QueryPerformanceFrequency(&__liFrequency);
        }
        QueryPerformanceCounter(&liPerformanceCount);
        return (uint64_t)(((double)liPerformanceCount.QuadPart / (double)__liFrequency.QuadPart)*1000.0);
    }
    else {
#	if COMPV_OS_WINDOWS_RT
        return epochMillis();
#	else
        return timeGetTime();
#	endif
    }
#elif defined(HAVE_CLOCK_GETTIME) || _POSIX_TIMERS > 0 || COMPV_OS_ANDROID
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (((uint64_t)ts.tv_sec)*(uint64_t)1000) + (((uint64_t)ts.tv_nsec) / (uint64_t)1000000);
#elif COMPV_OS_APPLE
    static mach_timebase_info_data_t __apple_timebase_info = { 0, 0 };
    if (__apple_timebase_info.denom == 0) {
        (void)mach_timebase_info(&__apple_timebase_info);
    }
    return (uint64_t)((mach_absolute_time() * __apple_timebase_info.numer) / (1e+6 * __apple_timebase_info.denom));
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (((uint64_t)tv.tv_sec)*(uint64_t)1000) + (((uint64_t)tv.tv_usec) / (uint64_t)1000);
#endif
}

// http://en.wikipedia.org/wiki/Network_Time_Protocol
uint64_t CompVTime::ntpMillis()
{
    struct timeval tv;
    gettimeofday(&tv, (struct timezone *)NULL);
    return ntpMillis(&tv);
}

uint64_t CompVTime::ntpMillis(const struct timeval *tv)
{
    static const unsigned long __epoch = 2208988800UL;
    static const double __ntp_scale_frac = 4294967295.0;

    uint64_t tv_ntp;
    uint64_t tv_usecs;

    if (!tv) {
        COMPV_DEBUG_ERROR("Invalid parameter");
        return 0;
    }

    tv_ntp = tv->tv_sec + __epoch;
#if 0 // ARM floating point calc issue (__aeabi_d2uiz)
    tv_usecs = (tv->tv_usec * 1e-6) * __ntp_scale_frac;
    return ((tv_ntp << 32) | (uint32_t)tv_usecs);
#else
    tv_usecs = ((uint64_t)tv->tv_usec * (uint64_t)__ntp_scale_frac) / (uint64_t)1000000;
    return ((((uint64_t)tv_ntp) << 32) | (uint32_t)tv_usecs);
#endif
}


COMPV_NAMESPACE_END()