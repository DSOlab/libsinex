#include "sinex.hpp"
#include "ggeodesy/units.hpp"
#include <cstdlib>

constexpr int max_lines_in_block = 10000;

int parse_site_id_line(const char *line, dso::sinex::SiteId &sid) noexcept {
  std::memcpy(sid.m_site_code, line + 1, 4);
  std::memcpy(sid.m_point_code, line + 6, 2);
  std::memcpy(sid.m_domes, line + 9, 9);
  sid.m_obs_code = line[19];
  std::memcpy(sid.m_description, line + 21, 22);

  int deg, mm;
  double sec;

  // parse DDD MM SSSS.S
  // TODO check for int/double parsing errors
  char *end;
  const char *start = line + 44;
  deg = std::strtol(start, &end, 10);
  start = end + 1;
  mm = std::strtol(start, &end, 10);
  start = end + 1;
  sec = std::strtod(start, &end);
  start = end + 1;
  sid.m_lon = dso::hexd2rad(deg, mm, sec, deg);

  deg = std::strtol(start, &end, 10);
  start = end + 1;
  mm = std::strtol(start, &end, 10);
  start = end + 1;
  sec = std::strtod(start, &end);
  start = end + 1;
  sid.m_lat = dso::hexd2rad(deg, mm, sec, deg);

  sid.m_hgt = std::strtod(start, &end);
  if (start == end) {
    fprintf(stderr,
            "[ERROR] Failed to parse SITE/ID line \'%s\' (traceback: %s)\n",
            line, __func__);
    return 1;
  }

  return 0;
}

int dso::Sinex::parse_block_site_id(std::vector<sinex::SiteId> &site_vec,
                                    int num_sites_requested,
                                    char **sites) noexcept {
  // clear the vector
  if (!site_vec.empty())
    site_vec.clear();
  if (site_vec.capacity() < (std::size_t)num_sites_requested)
    site_vec.reserve(num_sites_requested);

  // go to SITE/ID block
  if (goto_block("SITE/ID"))
    return 1;

  // next line to be read should be '+SITE/ID'
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SITE/ID")) {
    fprintf(stderr,
            "[ERROR] Expected \"%s\" line, found: \"%s\" (traceback: %s)\n",
            "+SITE/ID", line, __func__);
    return 1;
  }

  // read in SiteId's untill end of block
  std::size_t ln_count = 0;
  bool parse_line = true;
  while (m_stream.getline(line, sinex::max_sinex_chars) &&
         ++ln_count < max_lines_in_block) {
    parse_line = true;

    if (!std::strncmp(line, "-SITE/ID", 8))
      break;

    if (*line != '*') { // non-comment line

      // do we need to filter the stations ?
      if (num_sites_requested) {
        parse_line = false;
        for (int s = 0; s < num_sites_requested; s++) {
          if (!std::strncmp(sites[s], line + 1, 4)) {
            parse_line = true;
            break;
          }
        }
      }

      // do we need to parse the line ? ...
      if (parse_line) {
        site_vec.emplace_back(sinex::SiteId{});
        auto vecit = site_vec.end() - 1;
        if (parse_site_id_line(line, *vecit))
          return 1;
      } // if parse_line
    }   // non-comment line

    ++ln_count;
  }

  if (ln_count >= max_lines_in_block) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong! (traceback: %s)\n",
            ln_count, "-SITE/ID", __func__);
    return 1;
  }

  return 0;
}
