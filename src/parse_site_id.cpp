#include "sinex.hpp"
#include <cstdlib>

constexpr int max_lines_in_block = 10000;

int dso::Sinex::parse_block_site_id(std::vector<sinex::SiteId> &site_vec) noexcept {
  // clear the vector
  if (!site_vec.empty())
    site_vec.clear();

  // go to SITE/ID block
  if (goto_block("SITE/ID"))
    return 1;

  // next line to be read should be '+SITE/ID'
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SITE/ID")) {
    fprintf(stderr, "[ERROR] Expected \"%s\" line, found: \"%s\" (traceback: %s)\n", "+SITE/ID",
            line, __func__);
    return 1;
  }

  // read in SiteId's untill end of block
  std::size_t ln_count = 0;
  int deg, mm;
  double sec;
  while (m_stream.getline(line, sinex::max_sinex_chars) && ++ln_count < max_lines_in_block) {
    if (!std::strncmp(line, "-SITE/ID", 8))
      break;

    if (*line != '*') { // non-comment line
      site_vec.emplace_back(sinex::SiteId{});
      auto vecit = site_vec.end() - 1;

      std::memcpy(vecit->m_site_code, line + 1, 4);
      std::memcpy(vecit->m_point_code, line + 6, 2);
      std::memcpy(vecit->m_domes, line + 9, 9);
      vecit->m_obs_code = line[19];
      std::memcpy(vecit->m_description, line + 21, 22);

      // parse DDD MM SSSS.S
      // TODO check for int/double parsing errors
      char *end, *start = line + 44;
      deg = std::strtol(start, &end, 10);
      start = end + 1;
      mm = std::strtol(start, &end, 10);
      start = end + 1;
      sec = std::strtod(start, &end);
      start = end + 1;
      vecit->m_lon = ngpt::hexd2rad(deg, mm, sec, deg);

      deg = std::strtol(start, &end, 10);
      start = end + 1;
      mm = std::strtol(start, &end, 10);
      start = end + 1;
      sec = std::strtod(start, &end);
      start = end + 1;
      vecit->m_lat = ngpt::hexd2rad(deg, mm, sec, deg);

      vecit->m_hgt = std::strtod(start, &end);
      if (start == end) {
        fprintf(stderr, "[ERROR] Failed to parse SITE/ID line \'%s\' (traceback: %s)\n", line, __func__);
        return 1;
      }
    }
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
