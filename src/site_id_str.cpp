#include "sinex_blocks.hpp"
#include "geodesy/units.hpp"
#include <cstdio>

const char *dso::sinex::SiteId::to_str(char *buf) const noexcept {
  int d[6];
  double lon_sec, lat_sec;
  dso::rad2hexd(latitude(), d[1], d[2], lat_sec, d[0]);
  dso::rad2hexd(longitude(), d[4], d[5], lon_sec, d[3]);
  std::sprintf(buf, " %s %.2s %s %c %.22s %3d %2d %4.1f %3d %2d %4.1f %7.1f",
               site_code(), point_code(), domes(),
               SinexObservationCode_to_char(obscode()), description(),
               d[3] * d[4], d[5], lon_sec, d[0] * d[1], d[2], lat_sec,
               height());
  return buf;
}
