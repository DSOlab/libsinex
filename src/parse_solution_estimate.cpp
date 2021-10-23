#include "sinex.hpp"
#include <cstdlib>

constexpr int max_lines_in_block = 10000;

int parse_solution_estimate_line(
    const char *line, dso::sinex::SolutionEstimate &est,
    const dso::datetime<dso::seconds> &sinex_data_start) noexcept {
  char *end;
  est.m_index = std::strtol(line, &end, 10);
  if (end == line)
    return 1;

  std::memcpy(est.m_param_type, line + 7, 6);
  std::memcpy(est.m_site_code, line + 14, 4);
  std::memcpy(est.m_point_code, line + 19, 2);
  std::memcpy(est.m_soln_id, line + 22, 4);

  try {
    est.m_epoch = dso::sinex::parse_snx_date(line + 27);
  } catch (std::exception &) {
    fprintf(stderr,
            "[ERROR] Failed to parse date from line: \"%s\" (traceback: %s)\n",
            line, __func__);
    return 1;
  }
  if (est.m_epoch == dso::sinex::missing_sinex_date)
    est.m_epoch = sinex_data_start;

  std::memcpy(est.m_units, line + 40, 4);
  est.m_constraint_code = line[45];

  est.m_estimate = std::strtod(line + 47, &end);
  if (end == line + 47)
    return 1;

  est.m_std_deviation = std::strtod(line + 69, &end);
  if (end == line + 69)
    return 1;

  return 0;
}

int dso::Sinex::parse_block_solution_estimate(
    std::vector<sinex::SolutionEstimate> &site_vec) noexcept {
  // clear the vector
  if (!site_vec.empty())
    site_vec.clear();

  // go to SOLUTION/ESTIMATE  block
  if (goto_block("SOLUTION/ESTIMATE"))
    return 1;

  // next line to be read should be '+SOLUTION/ESTIMATE'
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SOLUTION/ESTIMATE")) {
    fprintf(stderr,
            "[ERROR] Expected \"%s\" line, found: \"%s\" (traceback: %s)\n",
            "+SOLUTION/ESTIMATE", line, __func__);
    return 1;
  }

  // read in SolutionEstimates's untill end of block
  std::size_t ln_count = 0;
  while (m_stream.getline(line, sinex::max_sinex_chars) &&
         ++ln_count < max_lines_in_block) {
    if (!std::strncmp(line, "-SOLUTION/ESTIMATE", 14))
      break;

    if (*line != '*') { // non-comment line
      site_vec.emplace_back(sinex::SolutionEstimate{});
      auto vecit = site_vec.end() - 1;
      if (parse_solution_estimate_line(line, *vecit, m_data_start))
        return 1;
    }

    ++ln_count;
  }

  if (ln_count >= max_lines_in_block) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong! (traceback: %s)\n",
            ln_count, "-SOLUTION/ESTIMATE", __func__);
    return 1;
  }

  return 0;
}

int dso::Sinex::parse_block_solution_estimate(
    std::vector<sinex::SolutionEstimate> &est_vec,
    const std::vector<sinex::SiteId> &site_vec) noexcept {
  // clear the vector
  if (!est_vec.empty())
    est_vec.clear();
  // we usually request a (kinda) state-vector for each site ...
  if (est_vec.capacity() < site_vec.size() * 6)
    est_vec.reserve(site_vec.size() * 6);

  // go to SOLUTION/ESTIMATE block
  if (goto_block("SOLUTION/ESTIMATE"))
    return 1;

  // next line to be read should be '+SOLUTION/ESTIMATE'
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SOLUTION/ESTIMATE")) {
    fprintf(stderr,
            "[ERROR] Expected \"%s\" line, found: \"%s\" (traceback: %s)\n",
            "+SOLUTION/ESTIMATE", line, __func__);
    return 1;
  }

  // read in SolutionEstimates's untill end of block
  std::size_t ln_count = 0;
  while (m_stream.getline(line, sinex::max_sinex_chars) &&
         ++ln_count < max_lines_in_block) {
    if (!std::strncmp(line, "-SOLUTION/ESTIMATE", 14))
      break;

    if (*line != '*') { // non-comment line

      // check if the site is of interest, aka included in site_vec
      auto it = std::find_if(
          site_vec.cbegin(), site_vec.cend(), [&](const sinex::SiteId &site) {
            return !std::strncmp(site.m_site_code, line + 14, 4) &&
                   !std::strncmp(site.m_point_code, line + 19, 2);
          });

      if (it != site_vec.cend()) { // collect estimate

        est_vec.emplace_back(sinex::SolutionEstimate{});
        auto vecit = est_vec.end() - 1;

        if (parse_solution_estimate_line(line, *vecit, m_data_start))
          return 1;
      }
    } // non-comment line
    ++ln_count;
  } // end of block

  if (ln_count >= max_lines_in_block) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong! (traceback: %s)\n",
            ln_count, "-SOLUTION/ESTIMATE", __func__);
    return 1;
  }

  return 0;
}
