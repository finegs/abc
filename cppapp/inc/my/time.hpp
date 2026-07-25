#pragma once

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <ctime>

#if defined(__MSC_VER)
#include <sys/timeb.h>
#include <windows.h>
#elif defined(__MINGW32__) || defined(__MINGW64__)
#include <sys/timeb.h>
#elif defined(__linux__)
#include <sys/time.h>
#endif

namespace my {

inline uint64_t curTimeMillis() {
#if defined(__linux__)
  struct timespec _t;
  clock_gettime(CLOCK_REALTIME, &_t);
  return _t.tv_sec * 1000 + lround(_t.tv_nsec / 1e6);
#elif defined(__MINGW32__) || defined(__MINGW64__)
  struct _timeb timebuffer;
  _ftime(&timebuffer);
  return static_cast<uint64_t>((timebuffer.time * 1000) + timebuffer.millitm);
#else
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  return static_cast<uint64_t>((ts.tv_sec * 1000) + ts.tv_nsec / 1e6);
#endif
}

inline long curTimeMillis_s(struct timespec *ts) {
#if defined(__linux__)
  clock_gettime(CLOCK_REALTIME, ts);
  return static_cast<long>(ts->tv_sec * 1000 + lround(ts->tv_nsec / 1e6));
#elif defined(__MINGW32__) || defined(__MINGW64__)
  struct _timeb timebuffer;
  _ftime(&timebuffer);
  return static_cast<long>((timebuffer.time * 1000) + timebuffer.millitm);
#else
  timespec_get(ts, TIME_UTC);
  return static_cast<long>((ts->tv_sec * 1000) + ts->tv_nsec / 1e6);
#endif
}

inline const char *tmstr(char *tstr) {
#if defined(__linux__)
  struct timespec ts;
  struct tm ti;
  clock_gettime(CLOCK_REALTIME, &ts);
  localtime_r((time_t *)&ts.tv_sec, &ti);
  strftime(tstr, 25, "%Y-%m-%d %H:%M:%S", &ti);
  sprintf(tstr, "%s.%03d", tstr, (int)lround(ts.tv_nsec / 1e6));
  return tstr;
#elif defined(__MINGW32__) || defined(__MINGW64__)
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  struct tm ti;
  localtime_s(&ti, &ts.tv_sec);
  strftime(tstr, 25, "%Y-%m-%d %H:%M:%S", &ti);
  sprintf(tstr, "%s.%03d", tstr, (int)lround(ts.tv_nsec / 1e6));
  return tstr;
#else
  struct tm st;
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  localtime_s(&st, (time_t *)&ts.tv_sec);

  sprintf_s(tstr, 32, "%04d-%02d-%02d %02d:%02d:%02d.%03u", st.tm_year + 1900,
            st.tm_mon + 1, st.tm_mday, st.tm_hour, st.tm_min, st.tm_sec,
            (unsigned int)(ts.tv_nsec / 1e6));
  return tstr;
#endif
}

} // namespace my
