#include "sinex.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s [SINEX_FILE]\n", argv[0]);
    return 1;
  }

  dso::Sinex snx(argv[1]);

#ifdef DEBUG
  snx.print_members();
#endif

#ifdef DEBUG
  snx.print_blocks();
#endif

  // parse STATION/RECEIVER block
  std::vector<dso::sinex::SiteReceiver> sites;
  if (snx.parse_block_site_receiver(sites)) {
    fprintf(stderr, "[ERROR] Failed parsing block \'SITE/RECEIVER\'\n");
    return 1;
  }

#ifdef DEBUG
  printf("Do you want to see details on the %zu collected sites? [y/N] ",
         sites.size());
  char yn;
  std::cin >> yn;
  if (yn == 'y' || yn == 'Y')
    for (auto const &s : sites)
      s.print();
#endif

  printf("All seem ok!\n");
  return 0;
}
