#include "sinex.hpp"
#include <cstdlib>

/// @brief Max line in SOLUTION/DATA_REJECT block
constexpr int max_lines_in_block = 100000;
/// @brief Start index for CODE in a SOLUTION/DATA_REJECT line
constexpr const int scode_start = 1;
/// @brief Start index for PT in a SOLUTION/DATA_REJECT line
constexpr const int spt_start = 6;
/// @brief Start index for SOLN in a SOLUTION/DATA_REJECT line
constexpr const int ssoln_start = 9;
/// @brief Start index for T in a SOLUTION/DATA_REJECT line
constexpr const int st_start = 14;
/// @brief Start index for M in a SOLUTION/DATA_REJECT line
constexpr const int sm_start = 42;
/// @brief Start index for A in a SOLUTION/DATA_REJECT line
constexpr const int sa_start = 44;
/// @brief Start index for DATA_START in a SOLUTION/DATA_REJECT line
constexpr const int sdata_start_start = 16;
/// @brief Start index for DATA_SEND in a SOLUTION/DATA_REJECT line
constexpr const int sdata_end_start = 29;
/// @brief Start index for COMMENTS in a SOLUTION/DATA_REJECT line
constexpr const int scomments_start = 46;

int parse_data_reject_line(
    const char *line, dso::sinex::RejectionInterval &rintrv,
    const dso::datetime<dso::seconds> &sinex_data_start,
    const dso::datetime<dso::seconds> &sinex_data_stop) noexcept {
  std::memcpy(rintrv.m_soln_id, line + ssoln_start, 4);
  rintrv.T = line[st_start];
  rintrv.M = line[sm_start];
  rintrv.A = line[sa_start];

  try {
    rintrv.start = dso::sinex::parse_snx_date(line + sdata_start_start);
  } catch (std::exception &) {
    fprintf(stderr,
            "[ERROR] Failed to parse date from line: \"%s\" (traceback: %s)\n",
            line, __func__);
    return 1;
  }
  try {
    rintrv.stop = dso::sinex::parse_snx_date(line + sdata_end_start);
  } catch (std::exception &) {
    fprintf(stderr,
            "[ERROR] Failed to parse date from line: \"%s\" (traceback: %s)\n",
            line, __func__);
    return 1;
  }
  if (rintrv.start == dso::sinex::missing_sinex_date)
    rintrv.start = sinex_data_start;
  if (rintrv.stop == dso::sinex::missing_sinex_date)
    rintrv.stop = sinex_data_stop;

  std::strcpy(rintrv.comments, line + scomments_start);

  return 0;
}

auto search_datareject_vec(const std::vector<dso::sinex::DataReject> &out_vec,
                           const char *line) {
  auto it = std::find_if(
      out_vec.begin(), out_vec.end(), [&](const dso::sinex::DataReject &site) {
        return !std::strncmp(site.m_site_code, line + scode_start, 4) &&
               !std::strncmp(site.m_point_code, line + spt_start, 2);
      });
  return it;
}

int dso::Sinex::parse_block_data_reject(
    std::vector<sinex::DataReject> &out_vec) noexcept {

  // clear the vector
  if (!out_vec.empty())
    out_vec.clear();

  // go to SOLUTION/ESTIMATE block
  if (goto_block("SOLUTION/DATA_REJECT"))
    return 1;

  // next line to be read should be '+SOLUTION/DATA_REJECT'
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SOLUTION/DATA_REJECT")) {
    fprintf(stderr,
            "[ERROR] Expected \"%s\" line, found: \"%s\" (traceback: %s)\n",
            "+SOLUTION/DATA_REJECT", line, __func__);
    return 1;
  }

  // read in DataReject's untill end of block
  std::size_t ln_count = 0;
  dso::sinex::RejectionInterval drIntrvl;
  while (m_stream.getline(line, sinex::max_sinex_chars) &&
         ++ln_count < max_lines_in_block) {
    if (!std::strncmp(line, "-SOLUTION/DATA_REJECT", 21))
      break;

    if (*line != '*') { // non-comment line

      if (parse_data_reject_line(line, drIntrvl, m_data_start, m_data_stop)) {
        return 1;
      }

      // check if we already have rejected intervals for this site
      auto out_vec_it = search_datareject_vec(
          out_vec, line); // this is an iterator to a const instance

      if (out_vec_it == out_vec.cend()) {
        sinex::DataReject newblock;
// GCC 8 added a -Wstringop-truncation warning but i want to do
// exactly this, so fuck off
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
        std::strncpy(newblock.m_site_code, line + scode_start, 4);
        std::strncpy(newblock.m_point_code, line + spt_start, 2);
#pragma GCC diagnostic pop
        newblock.add_rejection_interval(drIntrvl);
        out_vec.emplace_back(newblock);
      } else {
        // first cast the iterator to an iterator to a non-const instance ...
        auto ncit =
            out_vec.begin() + std::distance(out_vec.cbegin(), out_vec_it);
        ncit->add_rejection_interval(drIntrvl);
      }
    } // non-comment line
    ++ln_count;
  } // end of block

  if (ln_count >= max_lines_in_block) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong! (traceback: %s)\n",
            ln_count, "-SOLUTION/DATA_REJECT", __func__);
    return 1;
  }

  return 0;
}

int dso::Sinex::parse_block_data_reject(
    std::vector<sinex::DataReject> &out_vec,
    const std::vector<sinex::SiteId> &site_vec) noexcept {

  // clear the vector
  if (!out_vec.empty())
    out_vec.clear();

  // initial capacity, hopefully skip some allocs
  if (out_vec.capacity() < site_vec.size())
    out_vec.reserve(out_vec.size());

  // go to SOLUTION/ESTIMATE block
  if (goto_block("SOLUTION/DATA_REJECT"))
    return 1;

  // next line to be read should be '+SOLUTION/DATA_REJECT'
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SOLUTION/DATA_REJECT")) {
    fprintf(stderr,
            "[ERROR] Expected \"%s\" line, found: \"%s\" (traceback: %s)\n",
            "+SOLUTION/DATA_REJECT", line, __func__);
    return 1;
  }

  // read in DataReject's untill end of block
  std::size_t ln_count = 0;
  dso::sinex::RejectionInterval drIntrvl;
  while (m_stream.getline(line, sinex::max_sinex_chars) &&
         ++ln_count < max_lines_in_block) {
    if (!std::strncmp(line, "-SOLUTION/DATA_REJECT", 21))
      break;

    if (*line != '*') { // non-comment line

      // check if the site is of interest, aka included in site_vec
      auto it = std::find_if(
          site_vec.cbegin(), site_vec.cend(), [&](const sinex::SiteId &site) {
            return !std::strncmp(site.site_code(), line + scode_start, 4) &&
                   !std::strncmp(site.point_code(), line + spt_start, 2);
          });

      // the station is in the list
      if (it != site_vec.cend()) {

        if (parse_data_reject_line(line, drIntrvl, m_data_start, m_data_stop)) {
          return 1;
        }

        // check if we already have rejected intervals for this site
        auto out_vec_it = search_datareject_vec(
            out_vec, line); // this is an iterator to a const instance

        if (out_vec_it == out_vec.cend()) {
          sinex::DataReject newblock;
// GCC 8 added a -Wstringop-truncation warning but i want to do
// exactly this, so fuck off
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
          std::strncpy(newblock.m_site_code, line + scode_start, 4);
          std::strncpy(newblock.m_point_code, line + spt_start, 2);
#pragma GCC diagnostic pop
          newblock.add_rejection_interval(drIntrvl);
          out_vec.emplace_back(newblock);
        } else {
          // first cast the iterator to an iterator to a non-const instance ...
          auto ncit =
              out_vec.begin() + std::distance(out_vec.cbegin(), out_vec_it);
          ncit->add_rejection_interval(drIntrvl);
        }
      }
    } // non-comment line
    ++ln_count;
  } // end of block

  if (ln_count >= max_lines_in_block) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong! (traceback: %s)\n",
            ln_count, "-SOLUTION/DATA_REJECT", __func__);
    return 1;
  }

  return 0;
}
