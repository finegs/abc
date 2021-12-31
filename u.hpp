
#pragma once

#include <cstdio>
#include <sys/time.h>
#include <sys/timeb.h>
#include <time.h>
#include <cmath>

#ifdef __cplusplus__
extern "C" {}
#endif

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


#ifndef NDEBUG
#define DBG_LOG(msg) do { std::cout << tmstr(__tmstr) << " (" << __FILE__ << ":" << __LINE__ << "): " << msg << '\n'; } while(false)
#else
#define DEB_LOG(msg) do {} while {false}
#endif

long millis(){
    struct timespec _t;
    clock_gettime(CLOCK_REALTIME, &_t);
    return _t.tv_sec*1000 + lround(_t.tv_nsec/1e6);
}

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


#ifdef __cplusplus__
extern }
#endif
