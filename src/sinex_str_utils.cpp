#include "sinex_details.hpp"

const char *dso::sinex::bypass_wsc(const char *str, int max_count,
                                   int &chars_skipped) noexcept {
  chars_skipped = 0;
  const char *p = str;
  while (p && *p == ' ' && chars_skipped < max_count) {
    ++p;
    ++chars_skipped;
  }
  if (chars_skipped >= max_count)
    return nullptr;
  return p;
}

int dso::sinex::copy_untill_wsc(char *dest, const char *src,
                                int max_count) noexcept {
  int it = 0;
  const char *p = src;
  while (p && *p != ' ' && it < max_count) {
    *dest++ = *p++;
    ++it;
  }
  return it;
}