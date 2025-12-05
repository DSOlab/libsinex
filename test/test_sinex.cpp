#include "sinex.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

/* Test program: Creating SINEX files */

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s [SINEX_FILE]\n", argv[0]);
    return 1;
  }

  /* create an instance; could throw */
  try {
    dso::Sinex snx(argv[1]);
  } catch (std::exception &e) {
    fprintf(stderr, "ERROR. Failed to create SINEX instance from file %s\n",
            argv[1]);
    fprintf(stderr, "%s\n", e.what());
    return 1;
  }

  /* create an instance; should throw */
  try {
    dso::Sinex snx("foobar");
    return 1;
  } catch (std::exception &e) {
    ;
  }

  std::ofstream fout("deleteme");
  fout << "This is not\na SINEX file!";
  try {
    dso::Sinex snx("deleteme");
    return 1;
  } catch (std::exception &e) {
    ;
  }

  /* all done */
  return 0;
}
