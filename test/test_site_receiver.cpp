#include "sinex.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s [SINEX_FILE]\n", argv[0]);
    return 1;
  }

  dso::Sinex snx(argv[1]);

  /* parse STATION/RECEIVER block (all sites in SINEX) */
  std::vector<dso::sinex::SiteReceiver> sites;
  if (snx.parse_block_site_receiver(sites)) {
    fprintf(stderr, "[ERROR] Failed parsing block \'SITE/RECEIVER\'\n");
    return 1;
  }

  printf("All seem ok!\n");
  return 0;
}
