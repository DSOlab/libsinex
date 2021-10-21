#include "sinex.hpp"
#include <cstring>
#include <cstdio>

const char* bypass_wsc(const char *str, int max_count, int& chars_skipped) noexcept {
  chars_skipped = 0;
  const char *p = str;
  while (p && *p == ' ' && chars_skipped<max_count) {
    ++p;
    ++chars_skipped;
  }
  if (chars_skipped >= max_count) return nullptr;
  return p;
}

int copy_untill_wsc(const char *src, char *dest, int max_count) noexcept {
  int it = 0;
  const char *p = src;
  while (p && *p!= ' ' && it<max_count) {
    *dest++ = *p++;
    ++it;
  }
  return it;
}

int dso::sinex::SolutionEstimate::parse_line(const char *line) noexcept {
    const char *start=line;
    const char *cend;
    char *end;
    
    m_index = std::strtol(start, &end, 10);
    if (start == end) return 1;

    // parameter type
    start += 6;
    int chars_skipped = 0;
    int max_count = sizeof(m_param_type)/sizeof(m_param_type[0]) - 1;
    std::memset(m_param_type, 0, max_count+1);
    if (!copy_untill_wsc(bypass_wsc(start, max_count, chars_skipped), m_param_type, max_count-chars_skipped)) {
      fprintf(stderr, "[ERROR] Failed to parse parameter type from line \"%s\" (traceback: %s)\n", line, __func__);
      return 1;
    }

    // site code
    start += max_count + 1;
    max_count = sizeof(m_site_code)/sizeof(m_site_code[0]) - 1;
    std::memset(m_site_code, 0, max_count+1);
    if (!copy_untill_wsc(bypass_wsc(start, max_count, chars_skipped), m_site_code, max_count-chars_skipped)) {
      fprintf(stderr, "[ERROR] Failed to parse site code from line \"%s\" (traceback: %s)\n", line, __func__);
      return 1;
    }
    
    // point code
    start += max_count + 1;
    max_count = sizeof(m_point_code)/sizeof(m_point_code[0]) - 1;
    std::memset(m_point_code, 0, max_count+1);
    if (!copy_untill_wsc(bypass_wsc(start, max_count, chars_skipped), m_point_code, max_count-chars_skipped)) {
      fprintf(stderr, "[ERROR] Failed to parse point code from line \"%s\" (traceback: %s)\n", line, __func__);
      return 1;
    }
    
    // solution id
    start += max_count + 1;
    max_count = sizeof(m_soln_id)/sizeof(m_soln_id[0]) - 1;
    std::memset(m_soln_id, 0, max_count+1);
    if (!copy_untill_wsc(bypass_wsc(start, max_count, chars_skipped), m_soln_id, max_count-chars_skipped)) {
      fprintf(stderr, "[ERROR] Failed to parse solution id from line \"%s\" (traceback: %s)\n", line, __func__);
      return 1;
    }

    // epoch
    start += max_count + 1;
    try {
      m_epoch = parse_snx_date(start);
    } catch (std::runtime_error&) {
      fprintf(stderr, "[ERROR] Failed to parse date from line \"%s\" (traceback: %s)\n", line, __func__);
      return 1;
    }
    
    // units
    start += max_count + 1;
    max_count = sizeof(m_units)/sizeof(m_units[0]) - 1;
    std::memset(m_units, 0, max_count+1);
    if (!copy_untill_wsc(bypass_wsc(start, max_count, chars_skipped), m_units, max_count-chars_skipped)) {
      fprintf(stderr, "[ERROR] Failed to parse units from line \"%s\" (traceback: %s)\n", line, __func__);
      return 1;
    }
    max_count = 12;

    // constrain code
    start += max_count + 1;
    m_constraint_code = *start;
    max_count = 1;

    // estimate
    start += max_count + 1;
    m_estimate = std::strtod(start, &end);
    if (start == end) {
      fprintf(stderr, "[ERROR] Failed to parse estimate from line \"%s\" (traceback: %s)\n", line, __func__);
      return 1;
    }

    // std. deviation
    start = end + 1;
    m_std_deviation = std::strtod(start, &end);
    if (start == end) {
      fprintf(stderr, "[ERROR] Failed to parse std. deviation from line \"%s\" (traceback: %s)\n", line, __func__);
      return 1;
    }

    return 0;
}
