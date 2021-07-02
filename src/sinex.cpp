#include "sinex.hpp"
#include "ggeodesy/units.hpp"
#include <cstdlib>
#include <stdexcept>
#ifdef DEBUG
#include "ggdatetime/datetime_write.hpp"
#endif

/// @brief Resolve a string of type YY:DOY:SECOD to a datetime instance
/// @param[in] str A string of type: YY:DDD:SECOD, where yy is the year, DDD is
///            the day of year and SECOD the seconds of day. The string can 
///            have any number of whitespace characters at the begining. After
///            the SECOD field, the string must have a non-numeric character.
/// @return The datetime instance represented by the input string, in resolution
///         ngpt::seconds.
ngpt::datetime<ngpt::seconds> parse_snx_date(const char *str) {
  char *end;
  const char *start = str;
  int iyr = std::strtol(start, &end, 10);
  if (start == end || *end != ':') {
    throw std::runtime_error(
        "[ERROR] Failed resolving datetime in SINEX file (#1).\n");
  }
  iyr += (iyr <= 50) ? 2000 : 1900;
  start  = end + 1;
  int doy = std::strtol(start, &end, 10);
  if (start == end || *end != ':') {
    throw std::runtime_error(
        "[ERROR] Failed resolving datetime in SINEX file (#2).\n");
  }
  start = end + 1;
  long isc = std::strtol(start, &end, 10);
  if (start == end) {
    throw std::runtime_error(
        "[ERROR] Failed resolving datetime in SINEX file (#3).\n");
  }
  return ngpt::datetime<ngpt::seconds>{ngpt::year(iyr), ngpt::day_of_year(doy),
                                       ngpt::seconds(isc)};
}

Sinex::Sinex(const char *fn)
    : m_filename(std::string(fn)), m_stream(fn, std::ios::in) {
        if (this->mark_blocks()) {
          throw std::runtime_error("[ERROR] Failed to parse blocks in SINEX file\n");
        }
      }

int match_block_header(const char *str) noexcept {
  for (int i = 0; i < sinex::block_names_size; i++) {
    auto sz = std::strlen(sinex::block_names[i]);
    if (!std::strncmp(str, sinex::block_names[i], sz)) {
      return i;
    }
  }
  return -1;
}

int Sinex::mark_blocks() noexcept {
  if (!m_stream.is_open())
    return 1;
  m_blocks.clear();
  m_blocks.reserve(10);
  char line[sinex::max_sinex_chars];

  if (!m_stream.getline(line, sinex::max_sinex_chars)) {
    fprintf(stderr, "[ERROR] Failed parsing blocks; failed to read SINEX\n");
    return 1;
  }

  pos_t pos = m_stream.tellg();

  while (m_stream.getline(line, sinex::max_sinex_chars)) {
    if (*line == '+') {
      int idx = match_block_header(line + 1);
      if (idx < 0) {
        fprintf(stderr, "[ERROR] Could not match block with title \'%s\'\n",
                line + 1);
        return 1;
      }
#if __cplusplus > 201703L
      m_blocks.emplace_back(sinex::SinexBlockPosition{
          .mpos = pos, .mtype = sinex::block_names[idx]});
#else
      m_blocks.emplace_back(
          sinex::SinexBlockPosition{pos, sinex::block_names[idx]});
#endif
    }
    pos = m_stream.tellg();
  }

  if (!m_stream.eof()) {
    fprintf(stderr, "[ERROR] Seems SINEX was not read till EOF!\n");
    return 1;
  }
  m_stream.clear();
  return 0;
}

#ifdef DEBUG
void Sinex::print_blocks() const noexcept {
  for (const auto &b : this->m_blocks) {
    printf("\tSinex Block \"%s\"\n", b.mtype);
  }
}
#endif

#ifdef DEBUG
void Sinex::print_members() const {
  printf("Sinex Details:\n");
  printf("filename          %s\n", m_filename.c_str());
  printf("version           %5.2f\n", m_version);
  printf("agency            %s\n", m_agency);
  printf("created           %s\n",
         ngpt::strftime_ymd_hmfs(m_created_at).c_str());
  printf("data agency       %s\n", m_data_agency);
  printf("obs. code         %c\n", m_obs_code);
  printf("constraint code   %c\n", m_constraint_code);
  printf("solution contents %s\n", m_sol_contents);
  printf("num of estimated  %5d\n", m_num_estimates);
  printf("data start at     %s\n",
         ngpt::strftime_ymd_hmfs(m_data_start).c_str());
  printf("data end at       %s\n",
         ngpt::strftime_ymd_hmfs(m_data_stop).c_str());
}
#endif

int Sinex::parse_first_line() noexcept {
  char line[sinex::max_sinex_chars];
  char *end;

  if (!m_stream.is_open())
    return 1;

  m_stream.getline(line, sinex::max_sinex_chars);

  if (std::strncmp(line, "%=SNX", 5)) {
    fprintf(stderr, "[ERROR] Invalid first SINEX line\n");
    fprintf(stderr, "[ERROR] Expected field \'%%=SNX\' found \'%5s\'\n", line);
    return 1;
  }
  m_version = std::strtof(line + 5, &end);
  if (end == line + 5)
    return 1;
  std::memcpy(m_agency, line + 11, 3);
  try {
    m_created_at = parse_snx_date(line + 14);
  } catch (std::exception &) {
    return 1;
  }
  std::memcpy(m_data_agency, line + 28, 3);
  try {
    m_data_start = parse_snx_date(line + 31);
    m_data_stop = parse_snx_date(line + 44);
  } catch (std::exception &) {
    return 1;
  }
  m_obs_code = line[58];
  m_num_estimates = std::strtol(line + 60, &end, 10);
  if (end == line + 60)
    return 1;
  m_constraint_code = line[66];
  *m_sol_contents = line[68];
  // up to 6 available solution contents chars
  std::size_t lidx = 70, aidx = 1;
  while (lidx < std::strlen(line)) {
    m_sol_contents[aidx] = line[lidx];
    lidx += 2;
  }

  return 0;
}

int Sinex::parse_block_site_id(std::vector<sinex::SiteId> &site_vec) noexcept {
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
    fprintf(stderr, "[ERROR] Expected \"%s\" line, found: \"%s\"\n", "+SITE/ID",
            line);
    return 1;
  }

  // read in SiteId's untill end of block
  std::size_t ln_count = 0;
  int deg, mm;
  double sec;
  while (m_stream.getline(line, sinex::max_sinex_chars) && ++ln_count < 10000) {
    if (!std::strncmp(line, "-SITE/ID", 8))
      break;

    if (*line != '*') { /* non-comment line */
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
        fprintf(stderr, "[ERROR] Failed to parse SITE/ID line \'%s\'\n", line);
        return 1;
      }
    }
  }
  if (ln_count >= 10000) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong!\n",
            ln_count, "-SITE/ID");
    return 1;
  }

  return 0;
}

int Sinex::goto_block(const char *block) noexcept {
  m_stream.seekg(0);
  auto block_info_it = find_block(block);
  if (block_info_it == m_blocks.cend()) {
    fprintf(stderr, "[ERROR] Failed to locate block \'%s\' in parsed list\n",
            block);
    return 1;
  }
  m_stream.seekg(block_info_it->mpos, std::ios::beg);
  return 0;
}

int Sinex::parse_block_site_receiver(
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
    fprintf(stderr, "[ERROR] Expected \"%s\" line, found: \"%s\"\n",
            "+SITE/RECEIVER", line);
    return 1;
  }

  // read in SiteReceiver's untill end of block
  std::size_t ln_count = 0;
  while (m_stream.getline(line, sinex::max_sinex_chars) && ++ln_count < 10000) {
    if (!std::strncmp(line, "-SITE/RECEIVER", 14))
      break;

    if (*line != '*') { /* non-comment line */
      site_vec.emplace_back(sinex::SiteReceiver{});
      auto vecit = site_vec.end() - 1;

      std::memcpy(vecit->m_site_code, line + 1, 4);
      std::memcpy(vecit->m_point_code, line + 6, 2);
      std::memcpy(vecit->m_soln_id, line + 9, 4);
      vecit->m_obs_code = line[14];

      try {
        vecit->m_start = parse_snx_date(line + 16);
        vecit->m_stop = parse_snx_date(line + 29);
      } catch (std::exception &) {
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
  if (ln_count >= 10000) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong!\n",
            ln_count, "-SITE/RECEIVER");
    return 1;
  }

  return 0;
}

int Sinex::parse_block_site_antenna(
    std::vector<sinex::SiteAntenna> &site_vec) noexcept {
  // clear the vector
  if (!site_vec.empty())
    site_vec.clear();

  // go to SITE/ANTENNA  block
  if (goto_block("SITE/ANTENNA"))
    return 1;

  // next line to be read should be '+SITE/ANTENNA'
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SITE/ANTENNA")) {
    fprintf(stderr, "[ERROR] Expected \"%s\" line, found: \"%s\"\n",
            "+SITE/ANTENNA", line);
    return 1;
  }

  // read in SiteAntenna's untill end of block
  std::size_t ln_count = 0;
  while (m_stream.getline(line, sinex::max_sinex_chars) && ++ln_count < 10000) {
    if (!std::strncmp(line, "-SITE/ANTENNA", 14))
      break;

    if (*line != '*') { /* non-comment line */
      site_vec.emplace_back(sinex::SiteAntenna{});
      auto vecit = site_vec.end() - 1;

      std::memcpy(vecit->m_site_code, line + 1, 4);
      std::memcpy(vecit->m_point_code, line + 6, 2);
      std::memcpy(vecit->m_soln_id, line + 9, 4);
      vecit->m_obs_code = line[14];

      try {
        vecit->m_start = parse_snx_date(line + 16);
        vecit->m_stop = parse_snx_date(line + 29);
      } catch (std::exception &) {
        return 1;
      }
      if (vecit->m_start == sinex::missing_sinex_date)
        vecit->m_start = m_data_start;
      if (vecit->m_stop == sinex::missing_sinex_date)
        vecit->m_stop = m_data_stop;

      std::memcpy(vecit->m_ant_type, line + 42, 20);
      std::memcpy(vecit->m_ant_serial, line + 63, 5);
    }
  }
  if (ln_count >= 10000) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong!\n",
            ln_count, "-SITE/RECEIVER");
    return 1;
  }

  return 0;
}

int Sinex::parse_block_solution_estimate(
    std::vector<sinex::SolutionEstimate> &site_vec) noexcept {
  // clear the vector
  if (!site_vec.empty())
    site_vec.clear();

  // go to SITE/ANTENNA  block
  if (goto_block("SOLUTION/ESTIMATE"))
    return 1;

  // next line to be read should be '+SOLUTION/ESTIMATE'
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SOLUTION/ESTIMATE")) {
    fprintf(stderr, "[ERROR] Expected \"%s\" line, found: \"%s\"\n",
            "+SOLUTION/ESTIMATE", line);
    return 1;
  }

  // read in SolutionEstimates's untill end of block
  std::size_t ln_count = 0;
  char *end;
  while (m_stream.getline(line, sinex::max_sinex_chars) && ++ln_count < 10000) {
    if (!std::strncmp(line, "-SOLUTION/ESTIMATE", 14))
      break;

    if (*line != '*') { /* non-comment line */
      site_vec.emplace_back(sinex::SolutionEstimate{});
      auto vecit = site_vec.end() - 1;

      vecit->m_index = std::strtol(line, &end, 10);
      if (end == line)
        return 1;

      std::memcpy(vecit->m_param_type, line + 7, 6);
      std::memcpy(vecit->m_site_code, line + 14, 4);
      std::memcpy(vecit->m_point_code, line + 19, 2);
      std::memcpy(vecit->m_soln_id, line + 22, 4);

      try {
        vecit->m_epoch = parse_snx_date(line + 27);
      } catch (std::exception &) {
        return 1;
      }
      if (vecit->m_epoch == sinex::missing_sinex_date)
        vecit->m_epoch = m_data_start;

      std::memcpy(vecit->m_units, line + 40, 4);
      vecit->m_constraint_code = line[45];

      vecit->m_estimate = std::strtod(line + 47, &end);
      if (end == line + 47)
        return 1;
      vecit->m_std_deviation = std::strtod(line + 69, &end);
      if (end == line + 69)
        return 1;
    }
  }
  if (ln_count >= 10000) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong!\n",
            ln_count, "-SOLUTION/ESTIMATE");
    return 1;
  }

  return 0;
}

int Sinex::parse_block_solution_estimate(
    std::vector<sinex::SolutionEstimate> &est_vec,
    const std::vector<sinex::SiteId>& site_vec) noexcept {
  // clear the vector
  if (!est_vec.empty())
    est_vec.clear();

  // go to SITE/ANTENNA  block
  if (goto_block("SOLUTION/ESTIMATE"))
    return 1;

  // next line to be read should be '+SOLUTION/ESTIMATE'
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SOLUTION/ESTIMATE")) {
    fprintf(stderr, "[ERROR] Expected \"%s\" line, found: \"%s\"\n",
            "+SOLUTION/ESTIMATE", line);
    return 1;
  }

  // read in SolutionEstimates's untill end of block
  std::size_t ln_count = 0;
  char *end;
  while (m_stream.getline(line, sinex::max_sinex_chars) && ++ln_count < 10000) {
    if (!std::strncmp(line, "-SOLUTION/ESTIMATE", 14))
      break;

    if (*line != '*') { /* non-comment line */
      
      // check if the site is of interest, aka included in site_vec
      auto it = std::find_if(site_vec.cbegin(), site_vec.cend(), 
        [&](const sinex::SiteId& site) {
          return !std::strncmp(site.m_site_code, line+14, 4) && 
            !std::strncmp(site.m_point_code, line+19, 2);
        });
      
      if (it != site_vec.cend()) { /* collect etsimate */

        est_vec.emplace_back(sinex::SolutionEstimate{});
        auto vecit = est_vec.end() - 1;

        vecit->m_index = std::strtol(line, &end, 10);
        if (end == line)
          return 1;

        std::memcpy(vecit->m_param_type, line + 7, 6);
        std::memcpy(vecit->m_site_code, line + 14, 4);
        std::memcpy(vecit->m_point_code, line + 19, 2);
        std::memcpy(vecit->m_soln_id, line + 22, 4);

        try {
          vecit->m_epoch = parse_snx_date(line + 27);
        } catch (std::exception &) {
          return 1;
        }
        if (vecit->m_epoch == sinex::missing_sinex_date)
          vecit->m_epoch = m_data_start;

        std::memcpy(vecit->m_units, line + 40, 4);
        vecit->m_constraint_code = line[45];

        vecit->m_estimate = std::strtod(line + 47, &end);
        if (end == line + 47)
          return 1;
        vecit->m_std_deviation = std::strtod(line + 69, &end);
        if (end == line + 69)
          return 1;
      }
    }
  }
  if (ln_count >= 10000) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong!\n",
            ln_count, "-SOLUTION/ESTIMATE");
    return 1;
  }

  return 0;
}
