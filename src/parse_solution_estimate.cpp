#include "sinex.hpp"
#include <cstdlib>

constexpr int max_lines_in_block = 10000;

int dso::Sinex::parse_block_solution_estimate(
    std::vector<sinex::SolutionEstimate> &site_vec) noexcept {
  // clear the vector
  if (!site_vec.empty())
    site_vec.clear();

  // go to SITE/ANTENNA  block
  if (goto_block("SOLUTION/ESTIMATE"))
    return 1;

  // next line to be read should be '+SOLUTION/ESTIMATE'
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SOLUTION/ESTIMATE")) {
    fprintf(stderr, "[ERROR] Expected \"%s\" line, found: \"%s\" (traceback: %s)\n",
            "+SOLUTION/ESTIMATE", line, __func__);
    return 1;
  }

  // read in SolutionEstimates's untill end of block
  std::size_t ln_count = 0;
  char *end;
  while (m_stream.getline(line, sinex::max_sinex_chars) && ++ln_count < max_lines_in_block) {
    if (!std::strncmp(line, "-SOLUTION/ESTIMATE", 14))
      break;

    if (*line != '*') { // non-comment line
      site_vec.emplace_back(sinex::SolutionEstimate{});
      auto vecit = site_vec.end() - 1;

      vecit->m_index = std::strtol(line, &end, 10);
      if (end == line)
        return 1;

      std::memcpy(vecit->m_param_type, line + 7, 6);
      std::memcpy(vecit->m_site_code, line + 14, 4);
      std::memcpy(vecit->m_point_code, line + 19, 2);
      std::memcpy(vecit->m_soln_id, line + 22, 4);

      try {
        vecit->m_epoch = sinex::parse_snx_date(line + 27);
      } catch (std::exception &) {
        fprintf(stderr, "[ERROR] Failed to parse date from line: \"%s\" (traceback: %s)\n", line, __func__);
        return 1;
      }
      if (vecit->m_epoch == sinex::missing_sinex_date)
        vecit->m_epoch = m_data_start;

      std::memcpy(vecit->m_units, line + 40, 4);
      vecit->m_constraint_code = line[45];

      vecit->m_estimate = std::strtod(line + 47, &end);
      if (end == line + 47)
        return 1;
      vecit->m_std_deviation = std::strtod(line + 69, &end);
      if (end == line + 69)
        return 1;
    }
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
    const std::vector<sinex::SiteId>& site_vec) noexcept {
  // clear the vector
  if (!est_vec.empty())
    est_vec.clear();

  // go to SITE/ANTENNA  block
  if (goto_block("SOLUTION/ESTIMATE"))
    return 1;

  // next line to be read should be '+SOLUTION/ESTIMATE'
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SOLUTION/ESTIMATE")) {
    fprintf(stderr, "[ERROR] Expected \"%s\" line, found: \"%s\" (traceback: %s)\n",
            "+SOLUTION/ESTIMATE", line, __func__);
    return 1;
  }

  // read in SolutionEstimates's untill end of block
  std::size_t ln_count = 0;
  char *end;
  while (m_stream.getline(line, sinex::max_sinex_chars) && ++ln_count < 10000) {
    if (!std::strncmp(line, "-SOLUTION/ESTIMATE", 14))
      break;

    if (*line != '*') { // non-comment line
      
      // check if the site is of interest, aka included in site_vec
      auto it = std::find_if(site_vec.cbegin(), site_vec.cend(), 
        [&](const sinex::SiteId& site) {
          return !std::strncmp(site.m_site_code, line+14, 4) && 
            !std::strncmp(site.m_point_code, line+19, 2);
        });
      
      if (it != site_vec.cend()) { // collect estimate

        est_vec.emplace_back(sinex::SolutionEstimate{});
        auto vecit = est_vec.end() - 1;

        vecit->m_index = std::strtol(line, &end, 10);
        if (end == line)
          return 1;

        std::memcpy(vecit->m_param_type, line + 7, 6);
        std::memcpy(vecit->m_site_code, line + 14, 4);
        std::memcpy(vecit->m_point_code, line + 19, 2);
        std::memcpy(vecit->m_soln_id, line + 22, 4);

        try {
          vecit->m_epoch = sinex::parse_snx_date(line + 27);
        } catch (std::exception &) {
          fprintf(stderr, "[ERROR] Failed to parse date from line: \"%s\" (traceback: %s)\n", line, __func__);
          return 1;
        }
        if (vecit->m_epoch == sinex::missing_sinex_date)
          vecit->m_epoch = m_data_start;

        std::memcpy(vecit->m_units, line + 40, 4);
        vecit->m_constraint_code = line[45];

        vecit->m_estimate = std::strtod(line + 47, &end);
        if (end == line + 47)
          return 1;
        vecit->m_std_deviation = std::strtod(line + 69, &end);
        if (end == line + 69)
          return 1;
      }
    }
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