#include "sinex.hpp"
#include <cstdlib>
#include <charconv>

namespace {

const char *skip_ws(const char *line) noexcept {
  while (*line && *line == ' ') ++line;
  return line;
}

//*Code PT SOLN T Data_start__ Data_end____ AXE Up______ North___ East____        
// ADEA  A    1 D 93:003:00000 98:084:11545 UNE   0.5100   0.0000   0.0000        
int parse_eccentricity_line(const char *line, dso::sinex::SiteEccentricity & ecc, const dso::datetime<dso::seconds>& sinex_data_stop) noexcept {
  const int sz = std::strlen(line);
  if (sz < 70) return 9;

  auto ExtrapolateAfter(sinex_data_stop);
  ExtrapolateAfter.remove_seconds(dso::seconds(1));
  
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
  std::strncpy(ecc.m_id, line+1, 4);
  std::strncpy(ecc.pt, line+6, 2);
#pragma GCC diagnostic pop
  
  auto fc = std::from_chars(skip_ws(line+8), line+sz, ecc.soln);
  if (fc.ec != std::errc{}) return 8;
  
  const char *start = ++(fc.ptr);
  ecc.T = *fc.ptr;
  
  start = fc.ptr + 2;
  try {
    ecc.start = dso::sinex::parse_snx_date(start);
  } catch (std::exception &) {
    fprintf(stderr,
            "[ERROR] Failed to parse date from line: \"%s\" (traceback: %s)\n",
            line, __func__);
    return 7;
  }
  
  start += 13;
  try {
    ecc.stop = dso::sinex::parse_snx_date(start);
    if (ecc.stop >= ExtrapolateAfter) ecc.stop = dso::datetime<dso::seconds>::max();
  } catch (std::exception &) {
    fprintf(stderr,
            "[ERROR] Failed to parse date from line: \"%s\" (traceback: %s)\n",
            line, __func__);
    return 6;
  }

  start += 13;
  std::strncpy(ecc.axe, start, 3);
  
  start += 3;
  for (int i=0; i<3; i++) {
    fc = std::from_chars(skip_ws(start), line+sz, ecc.une[i]);
    if (fc.ec != std::errc{}) return 5;
    start = fc.ptr;
  }
  
  return 0;
}
}// unnamed namespace

int dso::Sinex::parse_block_site_eccentricity(
    std::vector<sinex::SiteEccentricity> &out_vec,
    const dso::datetime<dso::seconds> &t,
    const std::vector<sinex::SiteId> &site_vec) noexcept {

  // clear the vector
  if (!out_vec.empty())
    out_vec.clear();

  // initial capacity, hopefully skip some allocs
  if (out_vec.capacity() < site_vec.size())
    out_vec.reserve(out_vec.size());

  // go to SOLUTION/ECCENTRICITY block
  if (goto_block("SITE/ECCENTRICITY"))
    return 1;

  // next line to be read should be '+SITE/ECCENTRICITY'
  char line[sinex::max_sinex_chars];
  m_stream.getline(line, sinex::max_sinex_chars);
  if (!m_stream.good() || std::strcmp(line, "+SITE/ECCENTRICITY")) {
    fprintf(stderr,
            "[ERROR] Expected \"%s\" line, found: \"%s\" (traceback: %s)\n",
            "+SITE/ECCENTRICITY", line, __func__);
    return 1;
  }

  // read in Eccentricities untill end of block
  std::size_t ln_count = 0;
  constexpr const int max_lines_in_block = 5000;
  dso::sinex::SiteEccentricity secc;
  while (m_stream.getline(line, sinex::max_sinex_chars) &&
         ++ln_count < max_lines_in_block) {
    if (!std::strncmp(line, "-SITE/ECCENTRICITY", 22))
      break;

    if (*line != '*') { // non-comment line

      // parse the record line
      if (parse_eccentricity_line(line, secc, m_data_stop)) {
        fprintf(stderr, "[ERROR] Failed parsing eccentricity line: [%s] (traceback: %s)\n", line, __func__);
        return 1;
      }

      // check if the site is of interest, aka included in site_vec
      auto it = std::find_if(
          site_vec.cbegin(), site_vec.cend(), [&](const sinex::SiteId &site) {
            return !std::strncmp(site.m_site_code, secc.m_id, 4) &&
                   !std::strncmp(site.m_point_code, secc.pt, 2);
          });

      // the station is in the list and the time interval fits ...
      if (it != site_vec.cend() && (t>=secc.start && t<secc.stop)) {
        out_vec.push_back(secc);
      }
    } // non-comment line
   
   ++ln_count;
  } // end of block

  if (ln_count >= max_lines_in_block) {
    fprintf(stderr,
            "[ERROR] Read in %8zu lines and no \'%s\' line found .... smthng "
            "is wrong! (traceback: %s)\n",
            ln_count, "-SITE/ECCENTRICITY", __func__);
    return 1;
  }

  return 0;
}
