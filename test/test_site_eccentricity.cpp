#include "sinex.hpp"
#include <iostream>
#include <vector>

/*
 * Latest log file for DIONYSOS is available at:
 * https://ids-doris.org/network/sitelogs/station.html?code=DIONYSOS
 *
 * Name  Start         Stop
 * DIOA  15/02/1989    16/05/2006
 * DIOB  17/05/2006    -
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

  /* store eccentricities here */
  std::vector<dso::sinex::SiteEccentricity> ecc;

  {
    /* random date, for which we want eccentricities */
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(2020), dso::month(1), dso::day_of_month(1),
        dso::nanoseconds(0));
    /* get the eccentricities */
    if (snx.parse_block_site_eccentricity(siteids, t, ecc)) {
      fprintf(stderr, "Failed collecting site eccentricities\n");
      return 1;
    }
    /* report results */
    for (const auto &e : ecc) {
      printf("%s %.6f %.6f %.6f\n", e.site_code(), e.eccentricity(0),
             e.eccentricity(1), e.eccentricity(2));
    }
  }

  {
    /* reset random date, way into the future */
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(2030), dso::month(1), dso::day_of_month(1),
        dso::nanoseconds(0));
    /* get the eccentricities */
    if (snx.parse_block_site_eccentricity(siteids, t, ecc)) {
      fprintf(stderr, "Failed collecting site eccentricities\n");
      return 1;
    }
    /* report results */
    for (const auto &e : ecc) {
      printf("%s %.6f %.6f %.6f\n", e.site_code(), e.eccentricity(0),
             e.eccentricity(1), e.eccentricity(2));
    }
  }

  {
    /* reset random date, neither DIOA nor DIOB present */
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(2005), dso::day_of_year(349), dso::nanoseconds(0));
    /* get the eccentricities */
    if (snx.parse_block_site_eccentricity(siteids, t, ecc)) {
      fprintf(stderr, "Failed collecting site eccentricities\n");
      return 1;
    }
    /* report results */
    for (const auto &e : ecc) {
      printf("%s %.6f %.6f %.6f\n", e.site_code(), e.eccentricity(0),
             e.eccentricity(1), e.eccentricity(2));
    }
  }

  {
    /* reset random date, into the past (first day of DIOA) */
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(1993), dso::month(1), dso::day_of_month(3),
        dso::nanoseconds(0));
    /* get the eccentricities */
    if (snx.parse_block_site_eccentricity(siteids, t, ecc)) {
      fprintf(stderr, "Failed collecting site eccentricities\n");
      return 1;
    }
    /* report results */
    for (const auto &e : ecc) {
      printf("%s %.6f %.6f %.6f\n", e.site_code(), e.eccentricity(0),
             e.eccentricity(1), e.eccentricity(2));
    }
  }

  {
    /* reset random date, into the past (first day of DIOB) */
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(2006), dso::day_of_year(137), dso::nanoseconds(0));
    /* get the eccentricities */
    if (snx.parse_block_site_eccentricity(siteids, t, ecc)) {
      fprintf(stderr, "Failed collecting site eccentricities\n");
      return 1;
    }
    /* report results */
    for (const auto &e : ecc) {
      printf("%s %.6f %.6f %.6f\n", e.site_code(), e.eccentricity(0),
             e.eccentricity(1), e.eccentricity(2));
    }
  }

  return 0;
}
