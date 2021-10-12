#include "sinex.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s [SINEX_FILE]\n", argv[0]);
    return 1;
  }

  dso::Sinex snx(argv[1]);

  if (snx.parse_first_line()) {
    fprintf(stderr, "[ERROR] Failed parsing first line!\n");
    return 1;
  }

#ifdef DEBUG
  snx.print_members();
#endif

  snx.mark_blocks();
#ifdef DEBUG
  snx.print_blocks();
#endif

  // parse STATION/ID block
  std::vector<dso::sinex::SiteId> sites;
  if (snx.parse_block_site_id(sites)) {
    fprintf(stderr, "[ERROR] Failed parsing block \'SITE/ID\'\n");
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