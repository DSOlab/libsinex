#include "datetime/datetime_write.hpp"
#include "sinex.hpp"
#include <iostream>
#include <vector>

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

  /* store results here */
  std::vector<dso::sinex::DataReject> rej;

  {
    /* random datetime interval; collect data rejection periods that fall within
     * this interval
     */
    const auto t1 = dso::datetime<dso::nanoseconds>(
        dso::year(2020), dso::month(1), dso::day_of_month(1), dso::nanoseconds(0));
    const auto t2 = dso::datetime<dso::nanoseconds>(
        dso::year(2020), dso::month(2), dso::day_of_month(1), dso::nanoseconds(0));
    /* get data rejection info */
    if (snx.parse_block_data_reject(siteids, rej, t1, t2)) {
      fprintf(stderr, "Failed collecting data rejection info\n");
      return 1;
    }
    /* report results */
    char b1[64], b2[64];
    for (const auto &d : rej) {
      dso::strftime_ymd_hmfs(d.start, b1);
      dso::strftime_ymd_hmfs(d.stop, b2);
      printf("%s %s %s %s\n", d.site_code(), b1, b2, d.comment());
    }
  }

  {
    /* datetime interval; DIOA is rejected within this period */
    const auto t1 = dso::datetime<dso::nanoseconds>(
        dso::year(2005), dso::day_of_year(349), dso::nanoseconds(0));
    const auto t2 = dso::datetime<dso::nanoseconds>(
        dso::year(2005), dso::day_of_year(351), dso::nanoseconds(0));
    /* get data rejection info */
    if (snx.parse_block_data_reject(siteids, rej, t1, t2)) {
      fprintf(stderr, "Failed collecting data rejection info\n");
      return 1;
    }
    /* report results */
    char b1[64], b2[64];
    for (const auto &d : rej) {
      dso::strftime_ymd_hmfs(d.start, b1);
      dso::strftime_ymd_hmfs(d.stop, b2);
      printf("%s %s %s %s\n", d.site_code(), b1, b2, d.comment());
    }
  }

  {
    /* datetime interval; DIOA is rejected from start of this period to
     * 06:136:86399
     */
    const auto t1 = dso::datetime<dso::nanoseconds>(
        dso::year(2005), dso::day_of_year(350), dso::nanoseconds(0));
    /* get data rejection info */
    if (snx.parse_block_data_reject(siteids, rej, t1)) {
      fprintf(stderr, "Failed collecting data rejection info\n");
      return 1;
    }
    /* report results */
    char b1[64], b2[64];
    for (const auto &d : rej) {
      dso::strftime_ymd_hmfs(d.start, b1);
      dso::strftime_ymd_hmfs(d.stop, b2);
      printf("%s %s %s %s\n", d.site_code(), b1, b2, d.comment());
    }
  }

  {
    /* datetime interval; DIOA is rejected only at the last day of the given
     * interval
     */
    const auto t2 = dso::datetime<dso::nanoseconds>(
        dso::year(2005), dso::day_of_year(350), dso::nanoseconds(0));
    /* get data rejection info */
    if (snx.parse_block_data_reject(siteids, rej,
                                    dso::datetime<dso::nanoseconds>::min(), t2)) {
      fprintf(stderr, "Failed collecting data rejection info\n");
      return 1;
    }
    /* report results */
    char b1[64], b2[64];
    for (const auto &d : rej) {
      dso::strftime_ymd_hmfs(d.start, b1);
      dso::strftime_ymd_hmfs(d.stop, b2);
      printf("%s %s %s %s\n", d.site_code(), b1, b2, d.comment());
    }
  }

  return 0;
}
