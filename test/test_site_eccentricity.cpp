#include "sinex.hpp"
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>

/* DIOA  A    1 D 93:003:00000 06:136:86399 UNE   0.5100   0.0000   0.0000
 * DIOB  A    1 D 06:137:00000 22:365:86399 UNE   0.4870   0.0000   0.0000
 * 135/2006 is Tuesday,  May  16,  2006
 *
 MANB  A    1 D 03:056:00000 04:192:86399 UNE   0.4870   0.0000   0.0000
 MANB  A    2 D 04:193:00000 10:027:67773 UNE   0.4870   0.0000   0.0000
 MANB  A    3 D 10:027:67774 12:168:80327 UNE   0.4870   0.0000   0.0000
 MANB  A    4 D 12:168:80328 17:223:19705 UNE   0.4870   0.0000   0.0000
 MANB  A    5 D 17:223:19706 20:359:85421 UNE   0.4870   0.0000   0.0000
 MANB  A    6 D 20:359:85422 22:365:86399 UNE   0.4870   0.0000   0.0000
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
      printf("%s %.4f %.4f %.4f\n", e.site_code(), e.eccentricity(0),
             e.eccentricity(1), e.eccentricity(2));
    }
    /* should print
     * DIOB 0.4878 0.0000 0.0000
     * MANB 0.4870 0.0000 0.0000
     */
    assert(ecc.size() == 2);
    assert(!std::strcmp(ecc[0].point_code(), "A"));
    assert(!std::strcmp(ecc[1].point_code(), "A"));
    assert(!std::strcmp(ecc[0].soln_id(), "1"));
    assert(!std::strcmp(ecc[1].soln_id(), "5"));
    assert(!std::strcmp(ecc[0].ref_system(), "UNE"));
    assert(!std::strcmp(ecc[1].ref_system(), "UNE"));
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
      printf("%s %.4f %.4f %.4f\n", e.site_code(), e.eccentricity(0),
             e.eccentricity(1), e.eccentricity(2));
    }
    /* should print
     * DIOB 0.4870   0.0000   0.0000
     * MANB 0.4870   0.0000   0.0000
     */
    assert(ecc.size() == 2);
    assert(!std::strcmp(ecc[0].point_code(), "A"));
    assert(!std::strcmp(ecc[1].point_code(), "A"));
    assert(!std::strcmp(ecc[0].soln_id(), "1"));
    assert(!std::strcmp(ecc[1].soln_id(), "6"));
    assert(!std::strcmp(ecc[0].ref_system(), "UNE"));
    assert(!std::strcmp(ecc[1].ref_system(), "UNE"));
  }
  
  {
    /* reset random date, way into the future */
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(2030), dso::month(1), dso::day_of_month(1),
        dso::nanoseconds(0));
    /* get the eccentricities, NO EXTRAPOLATION! */
    if (snx.parse_block_site_eccentricity(siteids, t, ecc, false)) {
      fprintf(stderr, "Failed collecting site eccentricities\n");
      return 1;
    }
    /* report results */
    for (const auto &e : ecc) {
      printf("%s %.4f %.4f %.4f\n", e.site_code(), e.eccentricity(0),
             e.eccentricity(1), e.eccentricity(2));
    }
    /* should print nothing! */
    assert(ecc.size() == 0);
  }
  
  {
    /* reset random date, way into the future */
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(2022), dso::month(12), dso::day_of_month(31),
        dso::nanoseconds(86400000000000L));
    /* get the eccentricities, NO EXTRAPOLATION! */
    if (snx.parse_block_site_eccentricity(siteids, t, ecc, false)) {
      fprintf(stderr, "Failed collecting site eccentricities\n");
      return 1;
    }
    /* report results */
    for (const auto &e : ecc) {
      printf("%s %.4f %.4f %.4f\n", e.site_code(), e.eccentricity(0),
             e.eccentricity(1), e.eccentricity(2));
    }
    /* should print nothing! */
    assert(ecc.size() == 0);
  }

  {
    const auto t = dso::datetime<dso::nanoseconds>(
        dso::year(2005), dso::day_of_year(349), dso::nanoseconds(0));
    /* get the eccentricities */
    if (snx.parse_block_site_eccentricity(siteids, t, ecc)) {
      fprintf(stderr, "Failed collecting site eccentricities\n");
      return 1;
    }
    /* report results */
    for (const auto &e : ecc) {
      printf("%s %.4f %.4f %.4f\n", e.site_code(), e.eccentricity(0),
             e.eccentricity(1), e.eccentricity(2));
    }
    /* should print
     * DIOA 0.5100   0.0000   0.0000  
     * MANB 0.4870   0.0000   0.0000
     */
    assert(ecc.size() == 2);
    assert(!std::strcmp(ecc[0].point_code(), "A"));
    assert(!std::strcmp(ecc[1].point_code(), "A"));
    assert(!std::strcmp(ecc[0].soln_id(), "1"));
    assert(!std::strcmp(ecc[1].soln_id(), "2"));
    assert(!std::strcmp(ecc[0].ref_system(), "UNE"));
    assert(!std::strcmp(ecc[1].ref_system(), "UNE"));
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
      printf("%s %.4f %.4f %.4f\n", e.site_code(), e.eccentricity(0),
             e.eccentricity(1), e.eccentricity(2));
    }
    /* should print
     * DIOA 0.5100   0.0000   0.0000
     */
    assert(ecc.size() == 1);
    assert(!std::strcmp(ecc[0].point_code(), "A"));
    assert(!std::strcmp(ecc[0].soln_id(), "1"));
    assert(!std::strcmp(ecc[0].ref_system(), "UNE"));
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
      printf("%s %.4f %.4f %.4f\n", e.site_code(), e.eccentricity(0),
             e.eccentricity(1), e.eccentricity(2));
    }
    /* shuod print
     * DIOB 0.4870 0.0000 0.0000
     * MANB 0.4870 0.0000 0.0000
     */
    assert(ecc.size() == 2);
    assert(!std::strcmp(ecc[0].point_code(), "A"));
    assert(!std::strcmp(ecc[1].point_code(), "A"));
    assert(!std::strcmp(ecc[0].soln_id(), "1"));
    assert(!std::strcmp(ecc[1].soln_id(), "2"));
    assert(!std::strcmp(ecc[0].ref_system(), "UNE"));
    assert(!std::strcmp(ecc[1].ref_system(), "UNE"));
  }

  return 0;
}
