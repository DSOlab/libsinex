#include "sinex.hpp"
#include <charconv>
#include <cstdlib>
#include <stdexcept>

namespace {
constexpr int max_lines_in_block = 10000;
const char *skipws(const char *line) noexcept {
  while (*line && *line == ' ')
    ++line;
  return line;
}

int parse_solution_estimate_line(
    const char *line, dso::sinex::SolutionEstimate &est,
    const dso::datetime<dso::seconds> &sinex_data_start) noexcept {

  int error = 0, j;
  const char *end = line + std::strlen(line);

  /* parameter index */
  auto cv = std::from_chars(skipws(line), end, est.m_index);
  error += (cv.ec != std::errc{});

  /* parameter type */
  int index;
  if (dso::sinex::parameter_type_exists<
          dso::sinex::ParameterMatchPolicyType::NonStrict>(skipws(line + 7),
                                                           index)) {
    est.m_parameter_type = dso::sinex::parameter_types[index];
  } else {
    fprintf(stderr,
            "[ERROR] Failed matching parameter type in SINEX line \"%s\" "
            "(traceback: %s)\n",
            line, __func__);
    ++error;
  }

  std::memcpy(est.site_code(), line + 14, 4);
  std::memcpy(est.point_code(), line + 19, 2);
  std::memcpy(est.soln_id(), line + 22, 4);

  j = dso::sinex::parse_sinex_date(line + 27, sinex_data_start, est.m_epoch);
  if (j) {
    fprintf(stderr,
            "[ERROR] Failed parsing date in SINEX line \"%s\" "
            "(traceback: %s)\n",
            line, __func__);
  }
  error += j;

  std::memcpy(est.units(), line + 40, 4);
  try {
    est.m_constraint = dso::sinex::char_to_SinexConstraintCode(line[45]);
  } catch (std::exception &) {
    fprintf(stderr,
            "[ERROR] Failed to match SINEX constraint code in line \"%s\" "
            "(traceback: %s)\n",
            line, __func__);
    ++error;
  }

  j = 0;
  cv = std::from_chars(skipws(line + 47), end, est.m_estimate);
  j += (cv.ec != std::errc{});
  cv = std::from_chars(skipws(line + 69), end, est.m_std_deviation);
  j += (cv.ec != std::errc{});
  if (j) {
    fprintf(stderr,
            "[ERROR] Failed parsing parameter/std. deviation values from SINEX "
            "line \"%s\" (traceback: %s)\n",
            line, __func__);
  }

  return (error + j);
}
} /* anonymous namespace */

int dso::Sinex::parse_block_solution_estimate(
    const std::vector<sinex::SiteId> &site_vec,
    std::vector<sinex::SolutionEstimate> &est_vec) noexcept {

  /* clear the vector; allocate storage */
  if (!est_vec.empty())
    est_vec.clear();
  if (est_vec.capacity() < site_vec.size() * 6)
    est_vec.reserve(site_vec.size() * 6);

  /* go to SOLUTION/ESTIMATE block */
  if (goto_block("SOLUTION/ESTIMATE"))
    return 1;

  /* next line to be read should be '+SOLUTION/ESTIMATE' */
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SOLUTION/ESTIMATE")) {
    fprintf(stderr,
            "[ERROR] Expected \"%s\" line, found: \"%s\" (traceback: %s)\n",
            "+SOLUTION/ESTIMATE", line, __func__);
    return 1;
  }

  /* read in SolutionEstimates's untill end of block */
  std::size_t ln_count = 0;
  int error = 0;
  while (m_stream.getline(line, sinex::max_sinex_chars) &&
         (++ln_count < max_lines_in_block) && (!error)) {
    /* end of block encountered; break */
    if (!std::strncmp(line, "-SOLUTION/ESTIMATE", 14))
      break;

    if (*line != '*') { /* non-comment line */

      /* check if the site is of interest, aka included in site_vec */
      auto it = std::find_if(
          site_vec.cbegin(), site_vec.cend(), [&](const sinex::SiteId &site) {
            return !std::strncmp(site.site_code(), line + 14, 4) &&
                   !std::strncmp(site.point_code(), line + 19, 2);
          });

      if (it != site_vec.cend()) { /* parse and collect estimate */

        est_vec.emplace_back(sinex::SolutionEstimate{});
        auto vecit = est_vec.end() - 1;

        error = parse_solution_estimate_line(line, *vecit, m_data_start);
      }
    } /* non-comment line */
    ++ln_count;
  } /* end of block */

  /* check for infinite loop */
  if (ln_count >= max_lines_in_block) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong! (traceback: %s)\n",
            ln_count, "-SOLUTION/ESTIMATE", __func__);
    return 1;
  }

  /* check for parsing error */
  if (error) {
    fprintf(stderr, "[ERROR] Failed paring SINEX file %s (traceback: %s)\n",
            m_filename.c_str(), __func__);
    return 1;
  }

  return 0;
}
