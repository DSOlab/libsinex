#include "sinex.hpp"
#include <charconv>
#include <cstdio>

using SecIntType = typename dso::nanoseconds::underlying_type;
constexpr const SecIntType S2NS =
    dso::nanoseconds::template sec_factor<SecIntType>();

int dso::sinex::parse_sinex_date(
    const char *str, const dso::datetime<dso::nanoseconds> &tdefault,
    dso::datetime<dso::nanoseconds> &t) noexcept {
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

  if (error) {
    fprintf(stderr,
            "[ERROR] Failed to resolve SINEX date from string \"%s\" "
            "(traceback: %s)\n",
            str, __func__);
    return error;
  }

  t = tdefault;
  if (!((yr == 0) && (doy == 0) && (sec == 0))) {
    yr += (yr <= 50) ? 2000 : 1900;
    /* resolve datetime */
    try {
      t = dso::datetime<dso::nanoseconds>(dso::year(yr), dso::day_of_year(doy),
                                          dso::nanoseconds(sec * S2NS));
    } catch (std::exception &e) {
      fprintf(stderr,
              "[ERROR] Failed to resolve SINEX date from string \"%s\" "
              "(traceback: %s)\n",
              str, __func__);
      return 1;
    }
  }

  return 0;
}
