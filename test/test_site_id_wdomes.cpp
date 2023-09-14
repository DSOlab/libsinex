#include "sinex.hpp"
#include <cstdio>
#include <vector>
#include <cstring>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s [SINEX_FILE] [SITE1 DOMES1 ... SITEN DOMESN]\n",
            argv[0]);
    return 1;
  }

  dso::Sinex snx(argv[1]);

  std::vector<dso::sinex::SiteId> sitesids;
  /* collect SITE/ID info for given sites (no DOMES) */
  if (argc > 2) {
    
    int numsites = argc - 2;
    if (numsites%2) {
      fprintf(stderr, "ERROR Invalid usage. Each site should have a domes number!\n");
      return 1;
    }

    std::vector<const char *> sites;
    /* construct vector of SITE ID + SITE DOMES */
    for (int i = 2; i < argc; i+=2) {
      char *ptr = new char[24];
      std::memset(ptr, 0, 12);
      std::memcpy(ptr, argv[i], 4);
      std::memcpy(ptr+5, argv[i+1], 10);
      *(ptr+4) = ' ';
      sites.push_back(ptr);
    }
    for (const auto &p : sites) {printf("%s\n", p);}

    if (snx.parse_block_site_id(sitesids, sites, /*use_domes=*/true)) {
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
