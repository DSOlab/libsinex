#include "sinex.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s [SINEX_FILE] [SITE CODE]\n", argv[0]);
    return 1;
  }

  char site[4];
  std::strncpy(argv[2], site);
  printf("Searching for site \"%s\"\n", site);


  dso::Sinex snx(argv[1]);

  std::vector<dso::sinex::SiteId> site_vec;
  char *sites[] = &site;
  
  int error = snx.parse_block_site_id(site_vec, 1, sites);

  printf("Parsing block sites returned %d\n", error);
  return 0;
}
