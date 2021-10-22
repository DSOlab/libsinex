#include "sinex.hpp"
#include <cstring>

int dso::Sinex::get_solution_estimate(const char *site_codes[],
                                      const dso::datetime<dso::seconds> &t,
                                      bool error_if_missing = false) noexcept {

  // go to the block SOLUTION/ESTIMATE
  if (goto_block("SOLUTION/ESTIMATE")) {
    fprintf(stderr,
            "[ERROR] Failed to find block SOLUTION/ESTIMATE in SINEX file! "
            "(traceback: %s)\n",
            __func__);
    return 1;
  }

  // get lines iteratively (untill end of block) ...
  char line[sinex::max_sinex_chars];
  while (m_stream.getline(line, sinex::max_sinex_chars) &&
         std::strncmp("-SOLUTION/ESTIMATE", line, 18)) {
  }
}
