#include "sinex.hpp"
#include "ggeodesy/units.hpp"
#include <cstdlib>
#include <stdexcept>
#ifdef DEBUG
#include "ggdatetime/datetime_write.hpp"
#endif

constexpr long max_sinex_lines = 1'000'000;

dso::Sinex::Sinex(const char *fn)
    : m_filename(std::string(fn)), m_stream(fn, std::ios::in) {
  printf(">> initializing SINEX in constructor ... %s\n", __func__);
  if (this->mark_blocks()) {
    throw std::runtime_error("[ERROR] Failed to parse blocks in SINEX file\n");
  }
  printf("%s done\n", __func__);
}

int match_block_header(const char *str) noexcept {
  for (int i = 0; i < dso::sinex::block_names_size; i++) {
    auto sz = std::strlen(dso::sinex::block_names[i]);
    if (!std::strncmp(str, dso::sinex::block_names[i], sz)) {
      return i;
    }
  }
  return -1;
}

int dso::Sinex::mark_blocks() noexcept {
  if (!m_stream.is_open())
    return 1;

  m_blocks.clear();
  m_blocks.reserve(10);

  char line[sinex::max_sinex_chars];

  if (!m_stream.getline(line, sinex::max_sinex_chars)) {
    fprintf(
        stderr,
        "[ERROR] Failed parsing blocks; failed to read SINEX (traceback: %s)\n",
        __func__);
    return 1;
  }

  pos_t pos = m_stream.tellg();

  long linec = 0;
  while (m_stream.getline(line, sinex::max_sinex_chars) && linec++<max_sinex_lines) {
    if (*line == '+') {
      int idx = match_block_header(line + 1);
      if (idx < 0) {
        fprintf(
            stderr,
            "[ERROR] Could not match block with title \'%s\' (traceback: %s)\n",
            line + 1, __func__);
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

  if (!m_stream.eof() || linec>=max_sinex_lines) {
    fprintf(stderr,
            "[ERROR] Seems SINEX was not read till EOF! (traceback: %s)\n",
            __func__);
    return 1;
  }

  m_stream.clear();
  return 0;
}

#ifdef DEBUG
void dso::Sinex::print_blocks() const noexcept {
  for (const auto &b : this->m_blocks) {
    printf("\tSinex Block \"%s\"\n", b.mtype);
  }
}
#endif

#ifdef DEBUG
void dso::Sinex::print_members() const {
  printf("Sinex Details:\n");
  printf("filename          %s\n", m_filename.c_str());
  printf("version           %5.2f\n", m_version);
  printf("agency            %s\n", m_agency);
  printf("created           %s\n",
         dso::strftime_ymd_hmfs(m_created_at).c_str());
  printf("data agency       %s\n", m_data_agency);
  printf("obs. code         %c\n", m_obs_code);
  printf("constraint code   %c\n", m_constraint_code);
  printf("solution contents %s\n", m_sol_contents);
  printf("num of estimated  %5d\n", m_num_estimates);
  printf("data start at     %s\n",
         dso::strftime_ymd_hmfs(m_data_start).c_str());
  printf("data end at       %s\n", dso::strftime_ymd_hmfs(m_data_stop).c_str());
}
#endif

int dso::Sinex::parse_first_line() noexcept {
  char line[sinex::max_sinex_chars];
  char *end;

  if (!m_stream.is_open())
    return 1;

  // go to start of file
  m_stream.seekg(0);

  m_stream.getline(line, sinex::max_sinex_chars);

  if (std::strncmp(line, "%=SNX", 5)) {
    fprintf(stderr, "[ERROR] Invalid first SINEX line (traceback: %s)\n",
            __func__);
    fprintf(stderr, "[ERROR] Expected field \'%%=SNX\' found \'%5s\'\n", line);
    return 1;
  }

  m_version = std::strtof(line + 5, &end);
  if (end == line + 5)
    return 1;

  std::memcpy(m_agency, line + 11, 3);

  try {
    m_created_at = sinex::parse_snx_date(line + 14);
  } catch (std::exception &) {
    fprintf(stderr,
            "[ERROR] Failed to parse date from line: \"%s\" (traceback: %s)\n",
            line, __func__);
    return 1;
  }

  std::memcpy(m_data_agency, line + 28, 3);

  try {
    m_data_start = sinex::parse_snx_date(line + 31);
    m_data_stop = sinex::parse_snx_date(line + 44);
  } catch (std::exception &) {
    fprintf(stderr,
            "[ERROR] Failed to parse date from line: \"%s\" (traceback: %s)\n",
            line, __func__);
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

int dso::Sinex::goto_block(const char *block) noexcept {
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
