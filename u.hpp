#pragma once

#include <cstdio>
#include <ctime>
#include <cmath>
#include <sys/time.h>

#if defined(__cplusplus)
extern "C" {
#endif

<<<<<<< HEAD
inline long curTimeMillis()
{
    struct timespec _t;
    clock_gettime(CLOCK_REALTIME, &_t);
    return _t.tv_sec * 1000 + lround(_t.tv_nsec / 1e6);
}
=======
//const char* tmstr(char str[25]);
//#define TIME_UTC time(NULL)
//inline const char* tmstr(char str[25]) {
//
//    //time_t tm;
//    //struct timespec ts;
//    struct tm ti;
//	struct timeval tv;
//	time_t t;
//
//	gettimeofday(&tv, NULL);
//
//    //tm = time(NULL);
////    timespec_get(&ts, (int)TIME_UTC);
//    //ti = localtime(&tm);
//	t  = tv.tv_sec;
//    localtime_s(&ti, &t);
//    
//    strftime(str, 25, "%Y-%m-%d %H:%M:%S", &ti);
//    sprintf(str, "%s.%03ld", str, lround(tv.tv_usec/1e3));
//	return str;
//}
>>>>>>> 73f0e0f5734faf98792c4f4a451b99d7a1a8a913

inline long curTimeMillis_s(struct timespec *ts)
{
    clock_gettime(CLOCK_REALTIME, ts);
    return ts->tv_sec * 1000 + lround(ts->tv_nsec / 1e6);
}

inline const char* tmstr(char* tstr) {
	struct timespec ts; 
	struct tm ti;

	localtime_r((time_t*)&ts.tv_sec, &ti);

<<<<<<< HEAD
	strftime(tstr, 25, "%Y-%m-%d %H:%M:%S", &ti);
	sprintf(tstr, "%s.%03d", tstr, (int)lround(ts.tv_nsec/1e6));
	return tstr;
}
=======
inline long curTimeMillis()                                                                                           1 {
    struct timespec _t;
    clock_gettime(CLOCK_REALTIME, &_t);
    return _t.tv_sec * 1000 + lround(_t.tv_nsec / 1e6);
}

inline long curTimeMillis_s(struct timespec *ts)
{
    clock_gettime(CLOCK_REALTIME, ts);
    return ts->tv_sec * 1000 + lround(ts->tv_nsec / 1e6);
}

inline const char* tmstr(char tstr[25]) {
    struct timespec ts;
    struct tm ti;

    localtime_r((time_t*)&ts.tv_sec, &ti);

    strftime(tstr, 25, "%Y-%m-%d %H:%M:%S", &ti);
    sprintf(tstr, "%s.%03d", tstr, (int)lround(ts.tv_nsec/1e6));
    return tstr;
}

>>>>>>> 73f0e0f5734faf98792c4f4a451b99d7a1a8a913

#if defined(__cplusplus)
}
#endif

