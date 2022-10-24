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

#ifdef DEBUG
  snx.print_blocks();
#endif

  std::vector<dso::sinex::DataReject> rj;
  if ( snx.parse_block_data_reject(rj) ) {
    fprintf(stderr, "ERROR Failed to parse DATA_REJECT block\n");
    return 1;
  }

  printf("All seem ok!\n");
  return 0;
}
