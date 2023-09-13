#include "sinex.hpp"
#include <cstdlib>

namespace {
/* @brief Max line in SOLUTION/DATA_REJECT block */
constexpr int max_lines_in_block = 100000;
/* @brief Start index for CODE in a SOLUTION/DATA_REJECT line */
constexpr const int scode_start = 1;
/* @brief Start index for PT in a SOLUTION/DATA_REJECT line */
constexpr const int spt_start = 6;
/* @brief Start index for SOLN in a SOLUTION/DATA_REJECT line */
constexpr const int ssoln_start = 9;
/* @brief Start index for T in a SOLUTION/DATA_REJECT line */
constexpr const int st_start = 14;
/* @brief Start index for M in a SOLUTION/DATA_REJECT line */
constexpr const int sm_start = 42;
/* @brief Start index for A in a SOLUTION/DATA_REJECT line */
constexpr const int sa_start = 44;
/* @brief Start index for DATA_START in a SOLUTION/DATA_REJECT line */
constexpr const int sdata_start_start = 16;
/* @brief Start index for DATA_SEND in a SOLUTION/DATA_REJECT line */
constexpr const int sdata_end_start = 29;
/* @brief Start index for COMMENTS in a SOLUTION/DATA_REJECT line */
constexpr const int scomments_start = 46;

int parse_data_reject_line(
    const char *line, dso::sinex::DataReject &rintrv,
    const dso::datetime<dso::seconds> &sinex_data_start,
    const dso::datetime<dso::seconds> &sinex_data_stop) noexcept {
  int error = 0;
  std::memcpy(rintrv.soln_id(), line + ssoln_start, 4);
  try {
    rintrv.m_obscode = dso::sinex::char_to_SinexObservationCode(line[st_start]);
  } catch (std::exception &) {
    fprintf(stderr,
            "[ERROR] Erronuous SINEX Observation Code \'%c\' (traceback: %s)\n",
            line[st_start], __func__);
    ++error;
  }
  rintrv.colm() = line[sm_start];
  rintrv.cola() = line[sa_start];

  error += dso::sinex::parse_sinex_date(line + sdata_start_start,
                                        sinex_data_start, rintrv.start);
  error += dso::sinex::parse_sinex_date(line + sdata_end_start, sinex_data_stop,
                                        rintrv.stop);
  if (error) {
    fprintf(stderr,
            "[ERROR] Failed to parse date from line: \"%s\" (traceback: %s)\n",
            line, __func__);
  }

  std::strcpy(rintrv.comment(), line + scomments_start);

  return error;
}
} /* anonymous namespace */

int dso::Sinex::parse_block_data_reject(
    std::vector<sinex::DataReject> &out_vec,
    const std::vector<sinex::SiteId> &site_vec) noexcept {

  /* clear the vector, allocate storage */
  if (!out_vec.empty())
    out_vec.clear();
  if (out_vec.capacity() < site_vec.size())
    out_vec.reserve(out_vec.size());

  /* go to SOLUTION/ESTIMATE block */
  if (goto_block("SOLUTION/DATA_REJECT"))
    return 1;

  /* next line to be read should be '+SOLUTION/DATA_REJECT' */
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SOLUTION/DATA_REJECT")) {
    fprintf(stderr,
            "[ERROR] Expected \"%s\" line, found: \"%s\" (traceback: %s)\n",
            "+SOLUTION/DATA_REJECT", line, __func__);
    return 1;
  }

  /* read in DataReject's untill end of block */
  std::size_t ln_count = 0;
  int error = 0;
  dso::sinex::DataReject drIntrvl;
  while (m_stream.getline(line, sinex::max_sinex_chars) &&
         (++ln_count < max_lines_in_block) && (!error)) {
    /* end of block; break */
    if (!std::strncmp(line, "-SOLUTION/DATA_REJECT", 21))
      break;

    if (*line != '*') { /* non-comment line */

      /* check if the site is of interest, aka included in site_vec */
      auto it = std::find_if(
          site_vec.cbegin(), site_vec.cend(), [&](const sinex::SiteId &site) {
            return !std::strncmp(site.site_code(), line + scode_start, 4) &&
                   !std::strncmp(site.point_code(), line + spt_start, 2);
          });

      /* the station is in the list */
      if (it != site_vec.cend()) {
        /* parse line */
        error =
            parse_data_reject_line(line, drIntrvl, m_data_start, m_data_stop);
        /* add to list */
        out_vec.emplace_back(drIntrvl);
      }

    } /* non-comment line */
    ++ln_count;
  } /* end of block */

  /* check for infinite loop */
  if (ln_count >= max_lines_in_block) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong! (traceback: %s)\n",
            ln_count, "-SOLUTION/DATA_REJECT", __func__);
    return 1;
  }

  /* check for parsing errors */
  if (error) {
    fprintf(stderr, "[ERROR] Failed paring SINEX file %s (traceback: %s)\n",
            m_filename.c_str(), __func__);
    return 1;
  }

  return 0;
}
