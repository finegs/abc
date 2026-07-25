#pragma once

#include <cstddef>

namespace my {

inline size_t mstrhash(const char *s, size_t mod = 1024 * 1024) {
  size_t h = 5381UL;
  char c = '\0';
  while ('\0' != (c = s[0])) {
    h = ((h << 5) + h) + static_cast<size_t>(c); /* h*33+c */
    h %= mod;
    s++;
  }
  return h;
}

} // namespace my
