#pragma once

#include <cstdio>
#include <ctime>
#include <cstdarg>
#include <cmath>
#include <cstdint>
#if defined(__MSC_VER)
#include <windows.h>
#elif defined(__MINGW32__) || defined(__MINGW64__)
#include <sys/timeb.h>
#elif defined(__linux__)
#include <sys/time.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif


#ifndef NDEBUG
#define DBG_LOG(msg) do { std::cout << tmstr(__tmstr) << ":" << __FILE__ << ":" << __LINE__ << "): " << msg << '\n'; } while(false)
#else
#define DEB_LOG(msg) do {} while {false}
#endif

inline uint64_t curTimeMillis()
{
#if defined (__linux__)
    struct timespec _t;
    clock_gettime(CLOCK_REALTIME, &_t);
    return _t.tv_sec * 1000 + lround(_t.tv_nsec / 1e6);

#elif defined(_MINGW32__) || defined(__MINGW64__)
    struct _timeb timebuffer;
    _ftime(&timebuffer);
    return (uint64_t)(((timebuffer.time * 1000) + timebuffer.millitm));
#else
    struct timeb timebuffer;
    ftime(&timebuffer);
    return (uint64_t)(((timebuffer.time * 1000) + timebuffer.millitm));
#endif
}

inline long curTimeMillis_s(struct timespec *ts)
{
#if defined (__linux__)
    clock_gettime(CLOCK_REALTIME, ts);
    return ts->tv_sec * 1000 + lround(ts->tv_nsec / 1e6);
#elif defined(_MINGW32__) || defined(__MINGW64__)
    struct _timeb timebuffer;
    _ftime(&timebuffer);
    return (uint64_t)(((timebuffer.time * 1000) + timebuffer.millitm));
#else
    struct timeb timebuffer;
    ftime(&timebuffer);
    return (uint64_t)(((timebuffer.time * 1000) + timebuffer.millitm));
#endif
}

inline const char* tmstr(char* tstr) {
#if defined (__linux__)
	struct timespec ts; 
	struct tm ti;

	clock_gettime(CLOCK_REALTIME, &ts);
	localtime_r((time_t*)&ts.tv_sec, &ti);
	strftime(tstr, 25, "%Y-%m-%d %H:%M:%S", &ti);
	sprintf(tstr, "%s.%03d", tstr, (int)lround(ts.tv_nsec/1e6));
	return tstr;
#elif defined (__MINGW32__ ) || defined(__MINGW64__)

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	//localtime_r((time_t*)&ts.tv_sec, &ti);
	//time_t tt = time(NULL);
	struct tm ti;
	errno_t er;

	er =  localtime_s(&ti, &ts.tv_sec);
	strftime(tstr, 25, "%Y-%m-%d %H:%M:%S", &ti);
	sprintf(tstr, "%s.%03d", tstr, (int)lround(ts.tv_nsec/1e6));
	return tstr;
#if 0
	struct timeb tb;
	struct tm ti;
	ftime(&tb);
	localtime_s(&ti, &tb.time);
	strftime(tstr, 23, "%Y-%m-%d %H:%M:%S", &ti);
	sprintf(tstr, "%s.%03d", tstr, tb.millitm);
	return tstr;
#endif

#else
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf_s(tstr, "%4u:%02u:%02u %02u:%02u:%02u:%03u",
			st.wYear, st.wMonth, st.day, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return tstr;
#endif

}

static const char * __err_arg0 = "unknown";
void merr_setargs0(const char *arg0)
{
	__err_arg0 =  arg0;
}
void merr_error(const char* fmt, ...)
{
	va_list args;
	fprintf(stderr, "%s: ", __err_arg0);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(1);
}


#if defined(__cplusplus)
}
#endif

