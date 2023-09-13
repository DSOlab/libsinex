#include "sinex.hpp"
#include <charconv>
#include <cstdlib>
#include <stdexcept>
#ifdef DEBUG
#include "datetime/datetime_write.hpp"
#endif

namespace {
constexpr long max_sinex_lines = 1'000'000;
const char *skipws(const char *line) noexcept {
  while (*line && *line == ' ')
    ++line;
  return line;
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
} /* anonymous namespace */

dso::Sinex::Sinex(const char *fn)
    : m_filename(std::string(fn)), m_stream(fn, std::ios::in) {
  if (parse_first_line()) {
    throw std::runtime_error(
        "[ERROR] Failed to parse header line in SINEX file\n");
  }
  if (this->mark_blocks()) {
    throw std::runtime_error("[ERROR] Failed to parse blocks in SINEX file\n");
  }
}

int dso::Sinex::mark_blocks() noexcept {
  if (!m_stream.is_open())
    return 1;
  /* clear blocks and allocate storage */
  m_blocks.clear();
  m_blocks.reserve(10);

  char line[sinex::max_sinex_chars];
  pos_t pos = m_stream.tellg();
  int error = 0;
  long linec = 0;
  /* read SINEX lines through untill we reach '%ENDSNX' */
  while (m_stream.getline(line, sinex::max_sinex_chars) &&
         (linec++ < max_sinex_lines) && (!error)) {
    /* end of file; break */
    if (!std::strcmp(line, "%ENDSNX"))
      break;
    /* encounter start of block */
    if (*line == '+') {
      /* match it to a valid SINEX block */
      int idx = match_block_header(line + 1);
      if (idx < 0) {
        fprintf(
            stderr,
            "[ERROR] Could not match block with title \'%s\' (traceback: %s)\n",
            line + 1, __func__);
        ++error;
      }
      /* add end of previous line to m_blocks */
#if __cplusplus > 201703L
      m_blocks.emplace_back(sinex::SinexBlockPosition{
          .mpos = pos, .mtype = sinex::block_names[idx]});
#else
      m_blocks.emplace_back(
          sinex::SinexBlockPosition{pos, sinex::block_names[idx]});
#endif
    }
    /* update pos to be at the end of last line read */
    pos = m_stream.tellg();
  }

  /* check for errors */
  if ((!m_stream.good()) || error || (linec >= max_sinex_lines)) {
    if (!m_stream.good())
      fprintf(stderr,
              "[ERROR] Seems SINEX was not read till EOF! (traceback: %s)\n",
              __func__);
    if (error)
      fprintf(
          stderr,
          "[ERROR] Error occured while parsing SINEX file (traceback: %s)\n",
          __func__);
    if (linec >= max_sinex_lines)
      fprintf(stderr,
              "[ERROR] SINEX file has too many lines! (traceback: %s)\n",
              __func__);
    ++error;
  }

  /* clear the stream and return */
  m_stream.clear();
  return error;
}

int dso::Sinex::parse_first_line() noexcept {
  char line[sinex::max_sinex_chars];

  if (!m_stream.is_open())
    return 1;
  int error = 0;

  /* go to start of file, read in line */
  m_stream.seekg(0);
  m_stream.getline(line, sinex::max_sinex_chars);
  char *end = line + 80;

  if (std::strncmp(line, "%=SNX", 5)) {
    fprintf(stderr,
            "[ERROR] Invalid first SINEX line from %s (traceback: %s)\n",
            m_filename.c_str(), __func__);
    fprintf(stderr, "[ERROR] Expected field \'%%=SNX\' found \'%5s\'\n", line);
    ++error;
  }

  auto cv = std::from_chars(skipws(line + 5), end, m_version);
  if (cv.ec != std::errc{}) {
    fprintf(stderr,
            "[ERROR] Invalid first SINEX line from %s (traceback: %s)\n",
            m_filename.c_str(), __func__);
    fprintf(stderr, "[ERROR] Failed reading SINEX version (traceback: %s)",
            __func__);
    ++error;
  }

  std::memcpy(m_agency, line + 11, 3);

  if (sinex::parse_sinex_date(line + 14, dso::datetime<dso::seconds>::min(),
                              m_created_at)) {
    fprintf(stderr,
            "[ERROR] Invalid first SINEX line from %s (traceback: %s)\n",
            m_filename.c_str(), __func__);
    fprintf(stderr,
            "[ERROR] Failed to parse date from line: \"%s\" (traceback: %s)\n",
            line, __func__);
    ++error;
  }

  std::memcpy(m_data_agency, line + 28, 3);

  int j = 0;
  j += sinex::parse_sinex_date(line + 31, dso::datetime<dso::seconds>::min(),
                               m_data_start);
  j += sinex::parse_sinex_date(line + 44, dso::datetime<dso::seconds>::max(),
                               m_data_stop);
  if (error) {
    fprintf(stderr,
            "[ERROR] Invalid first SINEX line from %s (traceback: %s)\n",
            m_filename.c_str(), __func__);
    fprintf(stderr,
            "[ERROR] Failed to parse date from line: \"%s\" (traceback: %s)\n",
            line, __func__);
    error += j;
  }

  try {
    m_obscode = sinex::char_to_SinexObservationCode(line[58]);
  } catch (std::exception &) {
    fprintf(stderr,
            "[ERROR] Invalid first SINEX line from %s (traceback: %s)\n",
            m_filename.c_str(), __func__);
    fprintf(stderr,
            "[ERROR] Failed to translate SINEX observation code from \"%s\" "
            "(traceback: %s)\n",
            line, __func__);
    ++error;
  }

  cv = std::from_chars(skipws(line + 60), end, m_num_estimates);
  if (cv.ec != std::errc{}) {
    fprintf(stderr,
            "[ERROR] Invalid first SINEX line from %s (traceback: %s)\n",
            m_filename.c_str(), __func__);
    fprintf(stderr,
            "[ERROR] Failed to read number of estimates from \"%s\" "
            "(traceback: %s)\n",
            line, __func__);
  }

  try {
    m_constraint_code = dso::sinex::char_to_SinexConstraintCode(line[66]);
  } catch (std::exception &) {
    fprintf(stderr,
            "[ERROR] Invalid first SINEX line from %s (traceback: %s)\n",
            m_filename.c_str(), __func__);
    fprintf(stderr,
            "[ERROR] Failed to translate SINEX constraint code from \"%s\" "
            "(traceback: %s)\n",
            line, __func__);
    ++error;
  }

  *m_sol_contents = line[68];
  /* up to 6 available solution contents chars */
  std::size_t lidx = 70, aidx = 1;
  while (lidx < std::strlen(line)) {
    m_sol_contents[aidx] = line[lidx];
    lidx += 2;
  }

  return error;
}

int dso::Sinex::goto_block(const char *block) noexcept {
  /* find block by comparing strings */
  auto block_info_it = find_block(block);
  if (block_info_it == m_blocks.cend()) {
    fprintf(stderr, "[ERROR] Failed to locate block \'%s\' in parsed list\n",
            block);
    return 1;
  }
  /* rewind */
  m_stream.seekg(0);
  /* place the input stream at startt of previous line */
  m_stream.seekg(block_info_it->mpos, std::ios::beg);
  return 0;
}
