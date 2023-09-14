#include "sinex.hpp"
#include <cstdio>
#include <vector>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s [SINEX_FILE] [SITE1 SITE2 ... SITEN]\n",
            argv[0]);
    return 1;
  }

  dso::Sinex snx(argv[1]);

  /* parse STATION/ID block (all sites) */
  std::vector<dso::sinex::SiteId> sitesids;
  if (snx.parse_block_site_id(sitesids)) {
    fprintf(stderr, "ERROR Failed parsing block \'SITE/ID\'\n");
    return 1;
  }

  /* collect SITE/ID info for given sites (no DOMES) */
  if (argc > 2) {
    std::vector<const char *> sites;
    for (int i = 2; i < argc; i++) {
      sites.push_back(argv[i]);
    }
    if (snx.parse_block_site_id(sitesids, sites, /*use_domes=*/false)) {
      fprintf(stderr, "ERROR. Failed parsing SITE/ID from SINEX file\n");
      return 1;
    }

    for (const auto &site : sitesids) {
      printf("%s %s %s\n", site.site_code(), site.point_code(), site.domes());
    }
  }

  printf("All seem ok!\n");
  return 0;
}
