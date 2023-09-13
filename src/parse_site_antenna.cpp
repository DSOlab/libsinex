#include "sinex.hpp"
#include <cstdlib>

constexpr int max_lines_in_block = 10000;

int dso::Sinex::parse_block_site_antenna(
    std::vector<sinex::SiteAntenna> &site_vec) noexcept {
  /* clear the vector */
  if (!site_vec.empty())
    site_vec.clear();

  /* go to SITE/ANTENNA block */
  if (goto_block("SITE/ANTENNA"))
    return 1;

  /* next line to be read should be '+SITE/ANTENNA' */
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SITE/ANTENNA")) {
    fprintf(stderr,
            "[ERROR] Expected \"%s\" line, found: \"%s\" (traceback: %s)\n",
            "+SITE/ANTENNA", line, __func__);
    return 1;
  }

  /* read in SiteAntenna's untill end of block */
  std::size_t ln_count = 0;
  int error = 0;
  while (m_stream.getline(line, sinex::max_sinex_chars) &&
         (++ln_count < max_lines_in_block) && (!error)) {
    /* end of block; return */
    if (!std::strncmp(line, "-SITE/ANTENNA", 14))
      break;
    if (*line != '*') { /* non-comment line */
      site_vec.emplace_back(sinex::SiteAntenna{});
      auto vecit = site_vec.end() - 1;

      std::memcpy(vecit->site_code(), line + 1, 4);
      std::memcpy(vecit->point_code(), line + 6, 2);
      std::memcpy(vecit->soln_id(), line + 9, 4);
      try {
        vecit->m_obscode = dso::sinex::char_to_SinexObservationCode(line[14]);
      } catch (std::exception &) {
        fprintf(
            stderr,
            "[ERROR] Erronuous SINEX Observation Code \'%c\' (traceback: %s)\n",
            line[14], __func__);
        ++error;
      }

      error += sinex::parse_sinex_date(line + 16, m_data_start, vecit->m_start);
      error += sinex::parse_sinex_date(line + 29, m_data_stop, vecit->m_stop);
      if (error) {
        fprintf(
            stderr,
            "[ERROR] Failed to parse date from line: \"%s\" (traceback: %s)\n",
            line, __func__);
      }

      std::memcpy(vecit->ant_type(), line + 42, 20);
      std::memcpy(vecit->ant_serial(), line + 63, 5);
    }
  } /* end parsing block */

  /* check for infinite loop */
  if (ln_count >= max_lines_in_block) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong! (traceback :%s)\n",
            ln_count, "-SITE/RECEIVER", __func__);
    return 1;
  }

  /* check for error (while parsing) */
  if (error) {
    fprintf(stderr,
            "[ERROR] Failed to parse SITE/ANTENNA line from SINEX file %s "
            "(traceback: %s)\n",
            m_filename.c_str(), __func__);
    fprintf(stderr, "[ERROR] Line was \"%s\" (traceback: %s)\n", line,
            __func__);
    return 1;
  }

  return 0;
}
