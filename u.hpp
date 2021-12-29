
#pragma once

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
    struct timespec ts;
    struct tm ti;

    //tm = time(NULL);
    timespec_get(&ts, TIME_UTC);
    //ti = localtime(&tm);
    localtime_s(&ti, &ts.tv_sec);
    
    strftime(str, 25, "%Y-%m-%d %H:%M:%S", &ti);
    sprintf(str, "%s.%03ld", str, lround(ts.tv_nsec/1e6));
	return str;
}


#ifndef NDEBUG
#define DBG_LOG(msg) do { std::cout << tmstr(__tmstr) << ":" << __FILE__ << ":" << __LINE__ << "): " << msg << '\n'; } while(false)
#else
#define DEB_LOG(msg) do {} while {false}
#endif


#ifdef __cplusplus__
extern }
#endif
