#include "sinex_details.hpp"
#include <cstdlib>

/// @brief Resolve a string of type YY:DOY:SECOD to a datetime instance
/// @param[in] str A string of type: YY:DDD:SECOD, where yy is the year, DDD is
///            the day of year and SECOD the seconds of day. The string can 
///            have any number of whitespace characters at the begining. After
///            the SECOD field, the string must have a non-numeric character.
/// @return The datetime instance represented by the input string, in resolution
///         dso::seconds.
dso::datetime<dso::seconds> dso::sinex::parse_snx_date(const char *str) {
  char *end;
  const char *start = str;
  
  int iyr = std::strtol(start, &end, 10);
  if (start == end || *end != ':') {
    throw std::runtime_error(
        "[ERROR] Failed resolving datetime in SINEX file (#1).\n");
  }
  iyr += (iyr <= 50) ? 2000 : 1900;
  
  start  = end + 1;
  int doy = std::strtol(start, &end, 10);
  if (start == end || *end != ':') {
    throw std::runtime_error(
        "[ERROR] Failed resolving datetime in SINEX file (#2).\n");
  }

  start = end + 1;
  long isc = std::strtol(start, &end, 10);
  if (start == end) {
    throw std::runtime_error(
        "[ERROR] Failed resolving datetime in SINEX file (#3).\n");
  }
  
  return dso::datetime<dso::seconds>{dso::year(iyr), dso::day_of_year(doy),
                                       dso::seconds(isc)};
}
