#include "dpod.hpp"
#include "sinex.hpp"
#include <cstdio>

int main(int argc, char* argv[])
{
  if (argc < 4) {
    fprintf(stderr, "Usage: %s [SINEX_FILE] [dpod*_freq_corr.txt] [SITE1 ... SITEN]\n", argv[0]);
    return 2;
  }

  return 0;
}
