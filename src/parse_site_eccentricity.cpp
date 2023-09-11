#include "sinex.hpp"
#include <charconv>
#include <cstdlib>

namespace {

const char *skip_ws(const char *line) noexcept {
  while (*line && *line == ' ')
    ++line;
  return line;
}

/* Example Line:
 * Code PT SOLN T Data_start__ Data_end____ AXE Up______ North___ East____
 * ADEA  A    1 D 93:003:00000 98:084:11545 UNE   0.5100   0.0000   0.0000
 */
int parse_eccentricity_line(
    const char *line, dso::sinex::SiteEccentricity &ecc,
    const dso::datetime<dso::seconds> &sinex_data_start,
    const dso::datetime<dso::seconds> &sinex_data_stop) noexcept {
  /* don't even bother for sizes < 70 */
  const int sz = std::strlen(line);
  if (sz < 70)
    return 1;

  /* copy Site/Point Codes, Solution ID and Observation Code */
  std::strncpy(ecc.site_code(), line + 1, 4);
  std::strncpy(ecc.point_code(), line + 6, 2);
  std::strncpy(ecc.soln_id(), line + 9, 4);
  try {
    ecc.m_obscode = dso::sinex::char_to_SinexObservationCode(line[14]);
  } catch (std::exception &) {
    fprintf(stderr,
            "[ERROR] Erronuous SINEX Observation Code \'%c\' (traceback: %s)\n",
            line[19], __func__);
    return 1;
  }

  /* start/end dates */
  int error = 0;
  error += dso::sinex::parse_sinex_date(line + 16, sinex_data_start, ecc.start);
  error += dso::sinex::parse_sinex_date(line + 29, sinex_data_stop, ecc.stop);
  if (error) {
    fprintf(stderr,
            "[ERROR] Failed to parse date from line: \"%s\" (traceback: %s)\n",
            line, __func__);
  }

  /* ref system */
  std::strncpy(ecc.ref_system(), line + 42, 3);

  /* parse eccentricities */
  const char *start = line + 42 + 3;
  error = 0;
  for (int i = 0; i < 3; i++) {
    auto fc = std::from_chars(skip_ws(start), line + sz, ecc.une[i]);
    error += (fc.ec != std::errc{});
    start = fc.ptr;
  }

  return error;
}
} /* unnamed namespace */

int dso::Sinex::parse_block_site_eccentricity(
    std::vector<sinex::SiteEccentricity> &out_vec,
    const dso::datetime<dso::seconds> &t,
    const std::vector<sinex::SiteId> &site_vec) noexcept {

  /* clear the vector, and allocate */
  if (!out_vec.empty())
    out_vec.clear();
  if (out_vec.capacity() < site_vec.size())
    out_vec.reserve(out_vec.size());

  /* go to SOLUTION/ECCENTRICITY block */
  if (goto_block("SITE/ECCENTRICITY"))
    return 1;

  /* next line to be read should be '+SITE/ECCENTRICITY' */
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SITE/ECCENTRICITY")) {
    fprintf(stderr,
            "[ERROR] Expected \"%s\" line, found: \"%s\" (traceback: %s)\n",
            "+SITE/ECCENTRICITY", line, __func__);
    return 1;
  }

  /* define a datetime for comparing eccentricity validity intervals. If the
   * requested t (at input) is later than the SINEX's stop date, then this
   * datetime is one second prior to the SINEX's stop date
   */
  auto tstop = t;
  if (tstop > m_data_stop) {
    auto ExtrapolateAfter(m_data_stop);
    ExtrapolateAfter.remove_seconds(dso::seconds(1));
    tstop = ExtrapolateAfter;
  }

  /* read in Eccentricities untill end of block */
  std::size_t ln_count = 0;
  constexpr const int max_lines_in_block = 5000;
  int error = 0;
  dso::sinex::SiteEccentricity secc;
  while (m_stream.getline(line, sinex::max_sinex_chars) &&
         (++ln_count < max_lines_in_block) && (!error)) {
    /* end of block; break */
    if (!std::strncmp(line, "-SITE/ECCENTRICITY", 22))
      break;
    if (*line != '*') { /* non-comment line */

      /* parse the record line */
      if (parse_eccentricity_line(line, secc, m_data_start, m_data_stop)) {
        fprintf(
            stderr,
            "[ERROR] Failed parsing eccentricity line: [%s] (traceback: %s)\n",
            line, __func__);
        ++error;
      }

      /* check eccentricity validity interval */
      if (t >= secc.start && tstop < secc.stop) {

        /* check if the site is of interest, aka included in site_vec */
        auto it = std::find_if(
            site_vec.cbegin(), site_vec.cend(), [&](const sinex::SiteId &site) {
              return !std::strncmp(site.site_code(), secc.site_code(), 4) &&
                     !std::strncmp(site.point_code(), secc.point_code(), 2);
            });

        /* the station is in the list and the time interval fits ... */
        if (it != site_vec.cend()) {
          out_vec.push_back(secc);
        }
      } /* validity interval ok */
    }   /* non-comment line */

    ++ln_count;
  } /* end of block */

  /* check for infinite loop */
  if (ln_count >= max_lines_in_block) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong! (traceback: %s)\n",
            ln_count, "-SITE/ECCENTRICITY", __func__);
    return 1;
  }

  /* check for error (while parsing) */
  if (error) {
    fprintf(stderr,
            "[ERROR] Failed to parse SITE/ECCENTRICITY line from SINEX file %s "
            "(traceback: %s)\n",
            m_filename.c_str(), __func__);
    fprintf(stderr, "[ERROR] Line was \"%s\" (traceback: %s)\n", line,
            __func__);
    return 1;
  }

  return 0;
}
