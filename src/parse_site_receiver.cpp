#include "sinex.hpp"
#include <cstdlib>

constexpr int max_lines_in_block = 10000;

int dso::Sinex::parse_block_site_receiver(
    std::vector<sinex::SiteReceiver> &site_vec) noexcept {
  // clear the vector
  if (!site_vec.empty())
    site_vec.clear();

  // go to SITE/RECEIVER block
  if (goto_block("SITE/RECEIVER"))
    return 1;

  // next line to be read should be '+SITE/RECEIVER'
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SITE/RECEIVER")) {
    fprintf(stderr, "[ERROR] Expected \"%s\" line, found: \"%s\" (traceback: %s)\n",
            "+SITE/RECEIVER", line, __func__);
    return 1;
  }

  // read in SiteReceiver's untill end of block
  std::size_t ln_count = 0;
  while (m_stream.getline(line, sinex::max_sinex_chars) && ++ln_count < max_lines_in_block) {
    if (!std::strncmp(line, "-SITE/RECEIVER", 14))
      break;

    if (*line != '*') { // non-comment line
      site_vec.emplace_back(sinex::SiteReceiver{});
      auto vecit = site_vec.end() - 1;

      std::memcpy(vecit->m_site_code, line + 1, 4);
      std::memcpy(vecit->m_point_code, line + 6, 2);
      std::memcpy(vecit->m_soln_id, line + 9, 4);
      vecit->m_obs_code = line[14];

      try {
        vecit->m_start = sinex::parse_snx_date(line + 16);
        vecit->m_stop = sinex::parse_snx_date(line + 29);
      } catch (std::exception &) {
        fprintf(stderr, "[ERROR] Failed to parse date from line: \"%s\" (traceback: %s)\n", line, __func__);
        return 1;
      }
      if (vecit->m_start == sinex::missing_sinex_date)
        vecit->m_start = m_data_start;
      if (vecit->m_stop == sinex::missing_sinex_date)
        vecit->m_stop = m_data_stop;

      std::memcpy(vecit->m_rec_type, line + 42, 20);
      std::memcpy(vecit->m_rec_serial, line + 63, 5);
      std::memcpy(vecit->m_rec_firmware, line + 69, 11);
    }
  }

  if (ln_count >= max_lines_in_block) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong! (traceback: %s)\n",
            ln_count, "-SITE/RECEIVER", __func__);
    return 1;
  }

  return 0;
}
