#include "sinex.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s [SINEX_FILE] [SITE1 ... SITEN]\n", argv[0]);
    return 1;
  }

  dso::Sinex snx(argv[1]);

  /* place sites in a vector */
  std::vector<const char *> sites;
  for (int i = 2; i < argc; i++) {
    sites.push_back(argv[i]);
  }

  /* a vector of SiteId's to hold (intermediate) results */
  std::vector<dso::sinex::SiteId> siteids;

  /* parse the block SITE/ID to collect info for the given sites */
  if (snx.parse_block_site_id(sites, /*use domes=*/false, siteids)) {
    fprintf(stderr, "ERROR. Failed matching sites in SINEX file\n");
    return 1;
  }

  /* store Antennae infor here */
  std::vector<dso::sinex::SiteAntenna> antennae;

  {
    /* parse STATION/RECEIVER block */
    if (snx.parse_block_site_antenna(siteids, antennae)) {
      fprintf(stderr, "[ERROR] Failed parsing block \'SITE/ANTENNA\'\n");
      return 1;
    }
    /* report results */
    for (const auto &a : antennae) {
      printf("%s %s %s %s\n", a.site_code(), a.soln_id(), a.ant_type(),
             a.ant_serial());
    }
    /*
     * AREA 1 ALCATEL -----
     * AREA 2 ALCATEL -----
     * AREA 3 ALCATEL -----
     * AREA 4 ALCATEL -----
     * ARFB 1 STAREC-B -----
     * ARFB 2 STAREC-B -----
     * ARFB 3 STAREC-B -----
     * ARFB 4 STAREC-B -----
     * ARFB 5 STAREC-B -----
     * ARFB 6 STAREC-B -----
     * DIOA 1 ALCATEL -----
     * DIOB 1 STAREC-B -----
     * KESB 1 STAREC-B -----
     * KESB 2 STAREC-B -----
     * SANB 1 STAREC-B -----
     * SANB 2 STAREC-B -----
     * SANB 3 STAREC-B -----
     * SANB 4 STAREC-B -----
     * SANB 5 STAREC-B -----
     * SANB 6 STAREC-B -----
     * SANB 7 STAREC-B -----
     */
  }

  {
    const auto t1 = dso::datetime<dso::nanoseconds>(
        dso::year(2020), dso::month(1), dso::day_of_month(1),
        dso::nanoseconds(0));
    const auto t2 = dso::datetime<dso::nanoseconds>(
        dso::year(2020), dso::month(2), dso::day_of_month(1),
        dso::nanoseconds(0));
    /* parse STATION/RECEIVER block */
    if (snx.parse_block_site_antenna(siteids, antennae, t1, t2)) {
      fprintf(stderr, "[ERROR] Failed parsing block \'SITE/ANTENNA\'\n");
      return 1;
    }
    /* report results */
    for (const auto &a : antennae) {
      printf("%s %s %s %s\n", a.site_code(), a.soln_id(), a.ant_type(),
             a.ant_serial());
    }
    /*
     * ARFB 4 STAREC-B -----
     * DIOB 1 STAREC-B -----
     */
  }

  {
    const auto t1 = dso::datetime<dso::nanoseconds>(
        dso::year(2002), dso::month(1), dso::day_of_month(1),
        dso::nanoseconds(0));
    const auto t2 = dso::datetime<dso::nanoseconds>(
        dso::year(2003), dso::month(10), dso::day_of_month(1),
        dso::nanoseconds(0));
    /* parse STATION/RECEIVER block */
    if (snx.parse_block_site_antenna(siteids, antennae, t1, t2)) {
      fprintf(stderr, "[ERROR] Failed parsing block \'SITE/ANTENNA\'\n");
      return 1;
    }
    /* report results */
    for (const auto &a : antennae) {
      printf("%s %s %s %s\n", a.site_code(), a.soln_id(), a.ant_type(),
             a.ant_serial());
    }
    /*
     * DIOA A 1 ALCATEL -----
     * KESB A 1 STAREC-B -----
     * SANB A 1 STAREC-B -----
     * SANB A 2 STAREC-B -----
     */
  }

  /*
 AREA  A    1 D 93:003:00000 94:160:01996              ALCATEL -----
 AREA  A    2 D 94:160:01997 96:317:61184              ALCATEL -----
 AREA  A    3 D 96:317:61185 01:174:73994              ALCATEL -----
 AREA  A    4 D 01:174:73995 01:324:86399              ALCATEL -----
 ARFB  A    1 D 06:214:00000 08:190:33187             STAREC-B -----
 ARFB  A    2 D 08:190:33188 12:159:57780             STAREC-B -----
 ARFB  A    3 D 12:159:57781 16:336:81626             STAREC-B -----
 ARFB  A    4 D 16:336:81627 20:252:86399             STAREC-B -----
 ARFB  A    5 D 20:253:00000 20:366:86399             STAREC-B -----
 ARFB  A    6 D 21:001:00000 22:365:86399             STAREC-B -----
 DIOA  A    1 D 93:003:00000 06:136:86399              ALCATEL -----
 DIOB  A    1 D 06:137:00000 22:365:86399             STAREC-B -----
 KESB  A    1 D 01:094:00000 04:040:86399             STAREC-B -----
 KESB  A    2 D 04:041:00000 07:100:86399             STAREC-B -----
 SANB  A    1 D 01:059:00000 03:171:48641             STAREC-B -----
 SANB  A    2 D 03:171:48642 06:001:86399             STAREC-B -----
 SANB  A    3 D 06:002:00000 08:073:86399             STAREC-B -----
 SANB  A    4 D 08:074:00000 08:353:76768             STAREC-B -----
 SANB  A    5 D 08:353:76769 09:147:86399             STAREC-B -----
 SANB  A    6 D 09:148:00000 10:058:23654             STAREC-B -----
 SANB  A    7 D 10:059:00000 13:138:86399             STAREC-B -----
 */

  return 0;
}
