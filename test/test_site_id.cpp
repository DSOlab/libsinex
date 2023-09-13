#include "sinex.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s [SINEX_FILE]\n", argv[0]);
    return 1;
  }

  dso::Sinex snx(argv[1]);

  /* parse STATION/ID block */
  std::vector<dso::sinex::SiteId> sites;
  if (snx.parse_block_site_id(sites)) {
    fprintf(stderr, "[ERROR] Failed parsing block \'SITE/ID\'\n");
    return 1;
  }

  printf("All seem ok!\n");
  return 0;
}
