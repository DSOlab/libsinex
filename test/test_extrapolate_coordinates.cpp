#include "sinex.hpp"
#include <iostream>

int main(int argc, char *argv[]) {

  if (argc < 3) {
    fprintf(stderr, "Usage: %s [SINEX_FILE] [SITE1 ... SITEN]\n", argv[0]);
    return 1;
  }

  /* place sites in a vector */
  std::vector<const char *> sites;
  for (int i = 2; i < argc; i++) {
    sites.push_back(argv[i]);
  }

  /* create the sinex instance */
  dso::Sinex snx(argv[1]);

  /* a vector of SiteId's to hold (intermediate) results */
  std::vector<dso::sinex::SiteId> siteids;

  /* parse the block SITE/ID to collect info for the given sites */
  if (snx.parse_block_site_id(sites, /*use domes=*/false, siteids)) {
    fprintf(stderr, "ERROR. Failed matching sites in SINEX file\n");
    return 1;
  }

  /* hold results here */
  std::vector<dso::Sinex::SiteCoordinateResults> crd;

  { /* filter estimates based on date */
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(2007), dso::day_of_year(304), dso::nanoseconds(0));
    if (snx.linear_extrapolate_coordinates(siteids, t, crd)) {
      fprintf(stderr, "ERROR Failed extrapolating coordinate estimates\n");
      return 1;
    }
    for (auto const &e : crd)
      printf("%s %s %+.15f %+.15f %+.15f\n", e.msite.site_code(),
             e.msite.point_code(), e.x, e.y, e.z);
  }
  { /* filter estimates based on date */
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(2007), dso::day_of_year(305), dso::nanoseconds(0));
    if (snx.linear_extrapolate_coordinates(siteids, t, crd)) {
      fprintf(stderr, "ERROR Failed extrapolating coordinate estimates\n");
      return 1;
    }
    for (auto const &e : crd)
      printf("%s %s %+.15f %+.15f %+.15f\n", e.msite.site_code(),
             e.msite.point_code(), e.x, e.y, e.z);
  }
  { /* filter estimates based on date */
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(2030), dso::day_of_year(305), dso::nanoseconds(0));
    if (snx.linear_extrapolate_coordinates(siteids, t, crd)) {
      fprintf(stderr, "ERROR Failed extrapolating coordinate estimates\n");
      return 1;
    }
    for (auto const &e : crd)
      printf("%s %s %+.15f %+.15f %+.15f\n", e.msite.site_code(),
             e.msite.point_code(), e.x, e.y, e.z);
  }
  { /* filter estimates based on date */
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(1990), dso::day_of_year(305), dso::nanoseconds(0));
    if (snx.linear_extrapolate_coordinates(siteids, t, crd)) {
      fprintf(stderr, "ERROR Failed extrapolating coordinate estimates\n");
      return 1;
    }
    for (auto const &e : crd)
      printf("%s %s %+.15f %+.15f %+.15f\n", e.msite.site_code(),
             e.msite.point_code(), e.x, e.y, e.z);
  }
  { /* filter estimates based on date */
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(1990), dso::day_of_year(305),
        dso::nanoseconds(3 * 3600L * 1'000'000'000L));
    if (snx.linear_extrapolate_coordinates(siteids, t, crd)) {
      fprintf(stderr, "ERROR Failed extrapolating coordinate estimates\n");
      return 1;
    }
    for (auto const &e : crd)
      printf("%s %s %+.15f %+.15f %+.15f\n", e.msite.site_code(),
             e.msite.point_code(), e.x, e.y, e.z);
  }
  { /* filter estimates based on date */
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(1990), dso::day_of_year(305),
        dso::nanoseconds(6 * 3600L * 1'000'000'000L));
    if (snx.linear_extrapolate_coordinates(siteids, t, crd)) {
      fprintf(stderr, "ERROR Failed extrapolating coordinate estimates\n");
      return 1;
    }
    for (auto const &e : crd)
      printf("%s %s %+.15f %+.15f %+.15f\n", e.msite.site_code(),
             e.msite.point_code(), e.x, e.y, e.z);
  }
  { /* filter estimates based on date */
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(1990), dso::day_of_year(305),
        dso::nanoseconds(9 * 3600L * 1'000'000'000L));
    if (snx.linear_extrapolate_coordinates(siteids, t, crd)) {
      fprintf(stderr, "ERROR Failed extrapolating coordinate estimates\n");
      return 1;
    }
    for (auto const &e : crd)
      printf("%s %s %+.15f %+.15f %+.15f\n", e.msite.site_code(),
             e.msite.point_code(), e.x, e.y, e.z);
  }
  { /* filter estimates based on date */
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(1990), dso::day_of_year(305),
        dso::nanoseconds(12 * 3600L * 1'000'000'000L));
    if (snx.linear_extrapolate_coordinates(siteids, t, crd)) {
      fprintf(stderr, "ERROR Failed extrapolating coordinate estimates\n");
      return 1;
    }
    for (auto const &e : crd)
      printf("%s %s %+.15f %+.15f %+.15f\n", e.msite.site_code(),
             e.msite.point_code(), e.x, e.y, e.z);
  }


  return 0;
}
