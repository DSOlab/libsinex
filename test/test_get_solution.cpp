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
  std::vector<dso::sinex::SolutionEstimate> estimates;

  /* collect estimates for sites in siteids */
  if (snx.parse_block_solution_estimate(siteids, estimates)) {
    fprintf(stderr, "ERROR Failed parsing SOLUTION/ESTIMATES block\n");
    return 1;
  }
  for (auto const &e : estimates)
    printf("%s %s %.5f +/- %.5f\n", e.parameter_type(), e.site_code(),
           e.estimate(), e.std_deviation());

  { /* filter estimates based on date */
    printf("-------------------------------------------------------------\n");
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(2007), dso::day_of_year(304), dso::nanoseconds(0));
    if (snx.parse_block_solution_estimate(siteids, t, /*extrapolation*/ false,
                                          estimates)) {
      fprintf(stderr, "ERROR Failed filtering solution estimates\n");
      return 1;
    }
    for (auto const &e : estimates)
      printf("%s %s %.5f +/- %.5f\n", e.parameter_type(), e.site_code(),
             e.estimate(), e.std_deviation());
  }

  { /* filter estimates based on date */
    printf("-------------------------------------------------------------\n");
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(2007), dso::day_of_year(305), dso::nanoseconds(0));
    if (snx.parse_block_solution_estimate(siteids, t, /*extrapolation*/ false,
                                          estimates)) {
      fprintf(stderr, "ERROR Failed filtering solution estimates\n");
      return 1;
    }
    for (auto const &e : estimates)
      printf("%s %s %.5f +/- %.5f\n", e.parameter_type(), e.site_code(),
             e.estimate(), e.std_deviation());
  }

  { /* filter estimates based on date */
    printf("-------------------------------------------------------------\n");
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(2030), dso::day_of_year(305), dso::nanoseconds(0));
    if (snx.parse_block_solution_estimate(siteids, t, /*extrapolation*/ false,
                                          estimates)) {
      fprintf(stderr, "ERROR Failed filtering solution estimates\n");
      return 1;
    }
    for (auto const &e : estimates)
      printf("%s %s %.5f +/- %.5f\n", e.parameter_type(), e.site_code(),
             e.estimate(), e.std_deviation());
  }

  { /* filter estimates based on date */
    printf("-------------------------------------------------------------\n");
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(2030), dso::day_of_year(305), dso::nanoseconds(0));
    if (snx.parse_block_solution_estimate(siteids, t, /*extrapolation*/ true,
                                          estimates)) {
      fprintf(stderr, "ERROR Failed filtering solution estimates\n");
      return 1;
    }
    for (auto const &e : estimates)
      printf("%s %s %.5f +/- %.5f\n", e.parameter_type(), e.site_code(),
             e.estimate(), e.std_deviation());
  }

  { /* filter estimates based on date */
    printf("-------------------------------------------------------------\n");
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(1990), dso::day_of_year(305), dso::nanoseconds(0));
    if (snx.parse_block_solution_estimate(siteids, t, /*extrapolation*/ true,
                                          estimates)) {
      fprintf(stderr, "ERROR Failed filtering solution estimates\n");
      return 1;
    }
    for (auto const &e : estimates)
      printf("%s %s %.5f +/- %.5f\n", e.parameter_type(), e.site_code(),
             e.estimate(), e.std_deviation());
  }

  return 0;
}
