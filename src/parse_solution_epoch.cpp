#include "sinex.hpp"

namespace {
constexpr int max_lines_in_block = 10000;

int parse_epoch_line(const char *line,
                     const dso::datetime<dso::seconds> &sinex_data_start,
                     const dso::datetime<dso::seconds> &sinex_data_end,
                     dso::sinex::SolutionEpoch &entry) noexcept {
  int error = 0;
  std::memcpy(entry.site_code(), line + 1, 4);
  std::memcpy(entry.point_code(), line + 6, 2);
  std::memcpy(entry.soln_id(), line + 9, 4);
  try {
    entry.m_obscode = dso::sinex::char_to_SinexObservationCode(line[14]);
  } catch (std::exception &) {
    fprintf(stderr,
            "[ERROR] Erronuous SINEX Observation Code \'%c\' (traceback: %s)\n",
            line[14], __func__);
    ++error;
  }

  error +=
      dso::sinex::parse_sinex_date(line + 16, sinex_data_start, entry.m_start);
  error +=
      dso::sinex::parse_sinex_date(line + 29, sinex_data_end, entry.m_stop);
  error += dso::sinex::parse_sinex_date(
      line + 32, dso::datetime<dso::seconds>::min(), entry.m_mean);
  if (error) {
    fprintf(stderr,
            "[ERROR] Failed to parse date from line: \"%s\" (traceback: %s)\n",
            line, __func__);
  }

  return error;
}
} /* anonymous namespace */

int dso::Sinex::parse_solution_epoch(
    const dso::datetime<dso::seconds> &t,
    const std::vector<sinex::SiteId> &site_vec,
    std::vector<dso::sinex::SolutionEpoch> &out_vec) noexcept {
  /* clear the vector; allocate storage */
  if (!out_vec.empty())
    out_vec.clear();
  out_vec.reserve(site_vec.size());

  /* go to SOLUTION/EPOCHS block */
  if (goto_block("SOLUTION/EPOCHS"))
    return 1;

  /* next line to be read should be '+'SOLUTION/EPOCHS */
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SOLUTION/EPOCHS")) {
    fprintf(stderr,
            "[ERROR] Expected \"%s\" line, found: \"%s\" (traceback: %s)\n",
            "+SOLUTION/EPOCHS", line, __func__);
    return 1;
  }

  /* read in SOLUTION/EPOCHS records untill end of block */
  std::size_t ln_count = 0;
  int error = 0;
  dso::sinex::SolutionEpoch entry;
  while (m_stream.getline(line, sinex::max_sinex_chars) &&
         (++ln_count < max_lines_in_block) && (!error)) {
    /* end of block; return */
    if (!std::strncmp(line, "-SOLUTION/EPOCHS", 16))
      break;
    if (*line != '*') { /* non-comment line */
      /* check if the site is of interest, aka included in site_vec */
      auto it = std::find_if(
          site_vec.cbegin(), site_vec.cend(), [&](const sinex::SiteId &site) {
            return !std::strncmp(site.site_code(), line + 1, 4) &&
                   !std::strncmp(site.point_code(), line + 6, 2);
          });
      /* site is to be collected; parse line  */
      if (it != site_vec.cend()) {
        error = parse_epoch_line(line, m_data_start, m_data_stop, entry);
        /* check interval of solution */
        if (t >= entry.m_start && t < entry.m_stop && (!error)) {
          /* append epoch solution */
          out_vec.push_back(entry);
        } /* solution interval ok */
      }   /* site is in site_vec */
    }     /* mom-comment line */
  }       /* end parsing block */

  /* check for infinite loop */
  if (ln_count >= max_lines_in_block) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong! (traceback :%s)\n",
            ln_count, "-SOLUTION/EPOCHS", __func__);
    return 1;
  }

  /* check for error (while parsing) */
  if (error) {
    fprintf(stderr,
            "[ERROR] Failed to parse SOLUTION/EPOCHS line from SINEX file %s "
            "(traceback: %s)\n",
            m_filename.c_str(), __func__);
    fprintf(stderr, "[ERROR] Line was \"%s\" (traceback: %s)\n", line,
            __func__);
    return 1;
  }

  return 0;
}