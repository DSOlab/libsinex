#include "sinex.hpp"
#include <cstdio>
#include <cstring>
#include <vector>

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
    if (numsites % 2) {
      fprintf(stderr,
              "ERROR Invalid usage. Each site should have a domes number!\n");
      return 1;
    }

    std::vector<const char *> sites;
    /* construct vector of SITE ID + SITE DOMES */
    for (int i = 2; i < argc; i += 2) {
      char *ptr = new char[24];
      std::memset(ptr, 0, 12);
      std::memcpy(ptr, argv[i], 4);
      std::memcpy(ptr + 5, argv[i + 1], 10);
      *(ptr + 4) = ' ';
      sites.push_back(ptr);
    }
    // for (const auto &p : sites) {printf("%s\n", p);}

    if (snx.parse_block_site_id(sites, /*use_domes=*/true, sitesids)) {
      fprintf(stderr, "ERROR. Failed parsing SITE/ID from SINEX file\n");
      return 1;
    }

    for (const auto &site : sitesids) {
      printf("%s %s %s\n", site.site_code(), site.point_code(), site.domes());
    }
    /* expected output for input:
     * KOKO 123 FOO 91301S004 CRQC 91301S004 CRRC 91301S005 CRRC 91301S00 \
     * DAKA 34101S004 EASA 1703S008 EASA 41703S008 XAN 123 EASB 41703S009 \
     * EVEB 21501S001 EVEB 21501M001 DIOB 12602S012 DIOA 12602S011 DJIA
     * 39901S002 \ DIOA 12602S011 DJIC 39901S002 DJIB 39901S003 DIOA 12602S012
     * CRQC  A 91301S004
     * CRRC  A 91301S005
     * DAKA  A 34101S004
     * DIOA  A 12602S011
     * DIOB  A 12602S012
     * DJIA  A 39901S002
     * DJIB  A 39901S003
     * EASA  A 41703S008
     * EASB  A 41703S009
     * EVEB  A 21501S001
     */
  }

  return 0;
}
