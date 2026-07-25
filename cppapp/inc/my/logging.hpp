#pragma once

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <my/time.hpp>

#ifndef NDEBUG
#define DBG_LOG(msg)                                                           \
  do {                                                                         \
    char __tmstr[32]{'\0'};                                                    \
    std::cout << my::tmstr(__tmstr) << " : (" << __FILE__ << ":" << __LINE__   \
              << "), " << msg << '\n';                                         \
  } while (false)
#else
#define DBG_LOG(msg)                                                           \
  do {                                                                         \
  } while (false)
#endif

#ifndef NDEBUG
#define DBG_LOG2(...)                                                          \
  do {                                                                         \
    char __tmstr[32]{'\0'};                                                    \
    printf("%s %s:%d ", my::tmstr(__tmstr), __FILE__, __LINE__);               \
    printf(__VA_ARGS__);                                                       \
    printf("\n");                                                              \
  } while (0)
#else
#define DBG_LOG2(...)                                                          \
  do {                                                                         \
  } while (false)
#endif

namespace my {

static const char *__err_arg0 = "unknown";
inline void merr_setargs0(const char *arg0) { __err_arg0 = arg0; }
inline void merr_error(const char *fmt, ...) {
  va_list args;
  fprintf(stderr, "%s: ", __err_arg0);
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  exit(1);
}

} // namespace my
