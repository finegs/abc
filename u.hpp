
#pragma once

#include <cstdio>
#include <sys/time.h>
#include <sys/timeb.h>
#include <time.h>
#include <cmath>

#ifdef __cplusplus__
extern "C" {}
#endif

const char* tmstr(char str[25]);
#define TIME_UTC time(NULL)
inline const char* tmstr(char str[25]) {

    //time_t tm;
    //struct timespec ts;
    struct tm ti;
	struct timeval tv;
	time_t t;

	gettimeofday(&tv, NULL);

    //tm = time(NULL);
//    timespec_get(&ts, (int)TIME_UTC);
    //ti = localtime(&tm);
	t  = tv.tv_sec;
    localtime_s(&ti, &t);
    
    strftime(str, 25, "%Y-%m-%d %H:%M:%S", &ti);
    sprintf(str, "%s.%03ld", str, lround(tv.tv_usec/1e3));
	return str;
}


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


12  inline long curTimeMillis()                                                                                           1 {
  2     struct timespec _t;
  3     clock_gettime(CLOCK_REALTIME, &_t);
  4     return _t.tv_sec * 1000 + lround(_t.tv_nsec / 1e6);
  5 }
  6
  7 inline long curTimeMillis_s(struct timespec *ts)
  8 {
  9     clock_gettime(CLOCK_REALTIME, ts);
 10     return ts->tv_sec * 1000 + lround(ts->tv_nsec / 1e6);
 11 }
 12
 13 inline const char* tmstr(char* tstr) {
 14     struct timespec ts;
 15     struct tm ti;
 16
 17     localtime_r((time_t*)&ts.tv_sec, &ti);
 18
 19     strftime(tstr, 25, "%Y-%m-%d %H:%M:%S", &ti);
 20     sprintf(tstr, "%s.%03d", tstr, (int)lround(ts.tv_nsec/1e6));
 21     return tstr;
 22 }


#ifdef __cplusplus__
extern }
#endif
