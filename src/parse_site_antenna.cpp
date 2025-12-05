#include "sinex.hpp"
#include <cstdlib>

constexpr int max_lines_in_block = 10000;

int dso::Sinex::parse_block_site_antenna(
    const std::vector<sinex::SiteId> &site_vec,
    std::vector<sinex::SiteAntenna> &out_vec,
    const dso::datetime<dso::nanoseconds> from,
    const dso::datetime<dso::nanoseconds> to) noexcept {

  using sinex::details::ltrim_cpy;

  /* clear the vector */
  if (!out_vec.empty())
    out_vec.clear();

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

      /* first check site name */
      auto it = std::find_if(
          site_vec.cbegin(), site_vec.cend(), [&](const sinex::SiteId &site) {
            return !std::strncmp(site.site_code(), line + 1, sinex::SITE_CODE_CHAR_SIZE) &&
                   !std::strncmp(site.point_code(), line + 6, sinex::POINT_CODE_CHAR_SIZE);
          });

      /* the station is in the list */
      if (it != site_vec.cend()) {

        /* second, resolve time interval */
        dso::datetime<dso::nanoseconds> intrv_start, intrv_stop;
        error += sinex::parse_sinex_date(line + 16, m_data_start, intrv_start);
        error += sinex::parse_sinex_date(line + 29, m_data_stop, intrv_stop);
        if (error) {
          fprintf(stderr,
                  "[ERROR] Failed to parse date from line: \"%s\" (traceback: "
                  "%s)\n",
                  line, __func__);
        }

        /* if validity interval and antenna interval overlap */
        if (dso::intervals_overlap<
                dso::nanoseconds, dso::datetime_ranges::OverlapComparissonType::
                                      AllowEdgesOverlap>(
                intrv_start, intrv_stop, from, to)) {

          out_vec.emplace_back(sinex::SiteAntenna{});
          auto vecit = out_vec.end() - 1;

          //ltrim_cpy(vecit->site_code(), line + 1, 4);
          //ltrim_cpy(vecit->point_code(), line + 6, 2);
          //ltrim_cpy(vecit->soln_id(), line + 9, 4);
          std::memcpy(vecit->site_code(), line + 1, sinex::SITE_CODE_CHAR_SIZE);
          std::memcpy(vecit->point_code(), line + 6, sinex::POINT_CODE_CHAR_SIZE);
          std::memcpy(vecit->soln_id(), line + 9, sinex::SOLN_ID_CHAR_SIZE);
          try {
            vecit->m_obscode =
                dso::sinex::char_to_SinexObservationCode(line[14]);
          } catch (std::exception &) {
            fprintf(
                stderr,
                "[ERROR] Erronuous SINEX Observation Code \'%c\' (traceback: "
                "%s)\n",
                line[14], __func__);
            ++error;
          }

          ltrim_cpy(vecit->ant_type(), line + 42, 20);
          ltrim_cpy(vecit->ant_serial(), line + 63, 5);
        } /* intervals overlap */
      } /* station in the list */
    } /* non-comment line */
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
