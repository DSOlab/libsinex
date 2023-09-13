#include "sinex.hpp"
#include <iostream>
#include <stdexcept>

/* Test program: Creating SINEX files 
 *
 * Given valid SINEX file, this program should return 0
 * Given any non-SINEX file (including non-existent) it should throw and 
 * return 1.
 */ 

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s [SINEX_FILE]\n", argv[0]);
    return 1;
  }

  /* create an instance; could throw */
  try {
    dso::Sinex snx(argv[1]);
    return 0;
  } catch (std::exception &e) {
    fprintf(stderr, "ERROR. Failed to create SINEX instance from file %s\n",
            argv[1]);
    fprintf(stderr, "%s\n", e.what());
    return 1;
  }

  /* all done */
  printf("\n");
  return 0;
}
