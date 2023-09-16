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

  /* random datetime interval; collect data rejection periods that fall within
   * this interval
   */
  dso::datetime<dso::seconds> t1(dso::year(2020), dso::month(1),
                                 dso::day_of_month(1), dso::seconds(0));
  dso::datetime<dso::seconds> t2(dso::year(2020), dso::month(2),
                                 dso::day_of_month(1), dso::seconds(0));

  /* get data rejection info */
  std::vector<dso::sinex::DataReject> rej;
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

  printf("All seem ok!\n");
  return 0;
}
