#include "sinex.hpp"
#include <iostream>
#include <vector>

int main(int argc, char *argv[]) {

  if (argc < 3) {
    fprintf(stderr, "Usage: %s [SINEX_FILE] [SITE1 ... SITEN]\n",
            argv[0]);
    return 1;
  }

  /* place sites in a vector */
  std::vector<const char *> sites;
  for (int i = 2; i < argc; i++) {
    sites.push_back(argv[i]);
  }

  /* create the sinex instance */
  dso::Sinex snx(argv[1]);

  /* a vector of SiteId to hold (intermediate) results */
  std::vector<dso::sinex::SiteId> siteids;

  /* parse the block SITE/ID to collect info for the given sites */
  if (snx.parse_block_site_id(siteids, sites, /*use domes=*/false)) {
    fprintf(stderr, "ERROR. Failed matching sites in SINEX file\n");
    return 1;
  }
    
  /* random date, for which we want eccentricities */
  dso::datetime<dso::seconds> t(dso::year(2020), dso::month(1),
                                dso::day_of_month(1), dso::seconds(0));

  /* get the eccentricities */
  std::vector<dso::sinex::SiteEccentricity> ecc;
  if (snx.parse_block_site_eccentricity(ecc, t, siteids)) {
    fprintf(stderr, "Failed collecting site eccentricities\n");
    return 1;
  }

  /* report results */
  printf("Eccentricity per site:\n");
  for (const auto &e : ecc) {
    printf("%s %.6f %.6f %.6f\n", e.site_code(), e.une[0], e.une[1], e.une[2]);
  }

  printf("All seem ok!\n");
  return 0;
}
