#include "geodesy/units.hpp"
#include "sinex.hpp"
#include <charconv>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

namespace {
constexpr int max_lines_in_block = 10000;
inline const char *skipws(const char *line) noexcept {
  while (*line && *line == ' ')
    ++line;
  return line;
}

int parse_site_id_line(const char *line, dso::sinex::SiteId &sid) noexcept {
  int error = 0;
  std::memcpy(sid.site_code(), line + 1, 4);
  std::memcpy(sid.point_code(), line + 6, 2);
  std::memcpy(sid.domes(), line + 9, 9);
  try {
    sid.obscode() = dso::sinex::char_to_SinexObservationCode(line[19]);
  } catch (std::exception &) {
    fprintf(stderr,
            "[ERROR] Erronuous SINEX Observation Code \'%c\' (traceback: %s)\n",
            line[19], __func__);
    ++error;
  }
  std::memcpy(sid.description(), line + 21, 22);

  int deg, mm;
  double sec;

  /* parse DDD MM SSSS.S */
  const char *end = line + std::strlen(line);
  const char *start = line + 44;

  /* Longitude */
  auto cv = std::from_chars(skipws(start), end, deg);
  error += (cv.ec != std::errc());
  start = cv.ptr;
  cv = std::from_chars(skipws(start), end, mm);
  error += (cv.ec != std::errc());
  start = cv.ptr;
  cv = std::from_chars(skipws(start), end, sec);
  error += (cv.ec != std::errc());
  start = cv.ptr;
  if (error > 1) {
    fprintf(stderr, "[ERROR] Failed parsing site longitude (traceback: %s)\n",
            __func__);
    return error;
  }
  sid.longitude() = dso::hexd2rad(deg, mm, sec, deg);

  /* Latitude */
  cv = std::from_chars(skipws(start), end, deg);
  error += (cv.ec != std::errc());
  start = cv.ptr;
  cv = std::from_chars(skipws(start), end, mm);
  error += (cv.ec != std::errc());
  start = cv.ptr;
  cv = std::from_chars(skipws(start), end, sec);
  error += (cv.ec != std::errc());
  start = cv.ptr;
  if (error > 1) {
    fprintf(stderr, "[ERROR] Failed parsing site latitude (traceback: %s)\n",
            __func__);
    return error;
  }
  sid.latitude() = dso::hexd2rad(deg, mm, sec, deg);

  /* height */
  cv = std::from_chars(skipws(start), end, sid.height());
  error += (cv.ec != std::errc());
  if (error > 1) {
    fprintf(stderr, "[ERROR] Failed parsing site height (traceback: %s)\n",
            __func__);
    return error;
  }

  return 0;
}
} /* unnamed namespace */

int dso::Sinex::parse_block_site_id(
    const std::vector<const char *> &sites, bool use_domes,
    std::vector<sinex::SiteId> &site_vec) noexcept {
  /* clear the vector, alocate storage */
  if (!site_vec.empty())
    site_vec.clear();
  if (site_vec.capacity() < sites.size())
    site_vec.reserve(sites.size());

  /* go to SITE/ID block */
  if (goto_block("SITE/ID"))
    return 1;

  /* next line to be read should be '+SITE/ID' */
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SITE/ID")) {
    fprintf(stderr,
            "[ERROR] Expected \"%s\" line, found: \"%s\" (traceback: %s)\n",
            "+SITE/ID", line, __func__);
    return 1;
  }

  /* read in SiteId's untill end of block */
  std::size_t ln_count = 0;
  sinex::SiteId site;
  int error = 0;
  while (m_stream.getline(line, sinex::max_sinex_chars) &&
         (++ln_count < max_lines_in_block) && (!error)) {
    /* end of block encountered */
    if (!std::strncmp(line, "-SITE/ID", 8))
      break;
    if (*line != '*') { /* non-comment line */
      /* try to parse line */
      if (parse_site_id_line(line, site)) {
        fprintf(stderr,
                "[ERROR] Failed to parse SITE/ID line from SINEX file %s "
                "(traceback: %s)\n",
                m_filename.c_str(), __func__);
        fprintf(stderr, "[ERROR] Line was \"%s\" (traceback: %s)\n", line,
                __func__);
        ++error;
      }
      /* compare parse site to input ones */
      bool store_siteid = false;
      {
        for (const auto &ptr : sites) {
          const bool same =
              (use_domes)
                  ? (site.issame<sinex::SiteMatchPolicyType::USEDOMES>(ptr))
                  : (site.issame<sinex::SiteMatchPolicyType::IGNOREDOMES>(ptr));
          if (same) {
            store_siteid = true;
            break;
          }
        }
      }
      /* push_back if needed (if site is empty store anyway) */
      if (store_siteid || (!sites.size()))
        site_vec.push_back(site);
    } /* non-comment line */

    /* augment line count */
    ++ln_count;
  }

  /* check number of lines read */
  if (ln_count >= max_lines_in_block) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong! (traceback: %s)\n",
            ln_count, "-SITE/ID", __func__);
    return 1;
  }

  /* check for parsing errors */
  if (error) {
    fprintf(stderr,
            "[ERROR] Failed parsing block SITE/ID; erronuous line was: \'%s\' "
            "(traceback: %s)\n",
            line, __func__);
    return 1;
  }

  return 0;
}
