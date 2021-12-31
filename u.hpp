#pragma once

#include <cstdio>
#include <ctime>
#include <cmath>
#include <sys/time.h>

#if defined(__cplusplus)
extern "C" {
#endif

inline long curTimeMillis()
{
    struct timespec _t;
    clock_gettime(CLOCK_REALTIME, &_t);
    return _t.tv_sec * 1000 + lround(_t.tv_nsec / 1e6);
}

inline long curTimeMillis_s(struct timespec *ts)
{
    clock_gettime(CLOCK_REALTIME, ts);
    return ts->tv_sec * 1000 + lround(ts->tv_nsec / 1e6);
}

inline const char* tmstr(char* tstr) {
	struct timespec ts; 
	struct tm ti;

	localtime_r((time_t*)&ts.tv_sec, &ti);

	strftime(tstr, 25, "%Y-%m-%d %H:%M:%S", &ti);
	sprintf(tstr, "%s.%03d", tstr, (int)lround(ts.tv_nsec/1e6));
	return tstr;
}

#if defined(__cplusplus)
}
#endif

