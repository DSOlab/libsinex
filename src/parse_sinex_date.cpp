#include "sinex.hpp"
#include <charconv>
#include <cstdio>

int dso::sinex::parse_sinex_date(const char *str,
                                 const dso::datetime<dso::seconds> &tdefault,
                                 dso::datetime<dso::seconds> &t) noexcept {
  int yr, doy;
  long sec;
  int error = 0;

  /* skip leading whitespaces */
  while (*str && *str == ' ')
    ++str;

  /* end of string */
  const char *end = str + std::strlen(str);

  /* resolve numeric values year, doy, seconds of day */
  auto cr = std::from_chars(str, end, yr);
  error += (cr.ec != std::errc{});
  str = cr.ptr + 1;
  cr = std::from_chars(str, end, doy);
  error += (cr.ec != std::errc{});
  str = cr.ptr + 1;
  cr = std::from_chars(str, end, sec);
  error += (cr.ec != std::errc{});

  if (error)
    return error;

  t = tdefault;
  if (!((yr == 0) && (doy == 0) && (sec == 0))) {
    yr += (yr <= 50) ? 2000 : 1900;
    /* resolve datetime */
    t = dso::datetime<dso::seconds>(dso::year(yr), dso::day_of_year(doy),
                                    dso::seconds(sec));
  }

  return 0;
}
