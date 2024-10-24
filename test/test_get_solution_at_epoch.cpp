#include "datetime/calendar.hpp"
#include "sinex.hpp"
#include <datetime/date_integral_types.hpp>
#include <datetime/time_integral_types.hpp>
#include <iostream>

/*
 SANB  A    1 D 01:059:00000 03:171:48641
 SANB  A    2 D 03:171:48642 06:001:86399
 SANB  A    3 D 06:002:00000 08:073:86399
 SANB  A    4 D 08:074:00000 08:353:76768
 SANB  A    5 D 08:353:76769 09:147:86399
 SANB  A    6 D 09:148:00000 10:058:23654
 SANB  A    7 D 10:059:00000 13:138:86399
 DIOA  A    1 D 93:003:00000 06:136:86399
 DIOB  A    1 D 06:137:00000 22:365:86399
 */

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

  {
    dso::Datetime<dso::milliseconds> t(dso::year(2007), dso::day_of_year(73),
                                       dso::milliseconds(86399L * 1000));

    /* collect estimates for sites in siteids */
    if (snx.parse_block_solution_estimate(siteids, t, false, estimates)) {
      fprintf(stderr, "ERROR Failed parsing SOLUTION/ESTIMATES block\n");
      return 1;
    }
    for (auto const &e : estimates)
      printf("%s %s %s %.15e %.5e\n", e.parameter_type(), e.site_code(),
             e.soln_id(), e.estimate(), e.std_deviation());
  }

  return 0;
}
