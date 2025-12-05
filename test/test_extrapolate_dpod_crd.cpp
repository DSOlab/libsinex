#include "dpod.hpp"
#include <cstdio>
#include <vector>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

int main(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr,
            "Usage: %s [SINEX_FILE] [dpod*_freq_corr.txt] [SITE1 ... SITEN]\n",
            argv[0]);
    return 2;
  }

  /* place sites in a vector */
  std::vector<const char *> sites;
  for (int i = 3; i < argc; i++) {
    sites.push_back(argv[i]);
  }

  /* epoch t */
  const auto t = dso::datetime<dso::nanoseconds>(
      dso::year(2024), dso::day_of_year(304), dso::nanoseconds(0));

  assert(!dso::dpod_extrapolate(t, sites, argv[1], argv[2]));

  return 0;
}
