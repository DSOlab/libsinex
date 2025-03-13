#include "dpod.hpp"
#include <charconv>
#include <cstdio>
#include <fstream>
#include <limits>

namespace {

/** @brief Trim left whitespaces.
 *
 * Returns the same string starting from the first non-whitespace character.
 */
const char* skipws(const char* str) noexcept
{
  while (*str && *str == ' ')
    ++str;
  return str;
}

/** @brief Check if the give string, is a line of type:
 * '# Frequency  1 : 365.250 days'
 *
 * @param[in]  line The line to inspect.
 * @param[out] num_freq If True is returned, this is the number of frequency
 *             i.e. the index of frequency within the file, as recorded
 *             in the line.
 * @param[out] freq_days If True is returned, the the variable will hold the
 *             actual frequency, as recorded in the line.
 *
 * @return If False is returned the line was not (fully) resolved and hence
 *         does not hold a 'frequency line' string. num_freq and freq_days
 *         hold dummy values.
 *         If True is returned, then the line was successefully resolved,
 *         (indeed it represented a 'frequency line') and the variables
 *         num_freq and freq_days hold the 'data' of the frequency.
 */
int is_new_frequency_line(const char* line, int& num_freq, double& freq_days) noexcept
{
  if (line[0] != '#')
    return 0;
  /* se if we can match the string 'Frequency' after some whitespaces */
  int ofst = 1;
  while (*(line + ofst) && (*(line + ofst) == ' ' || *(line + ofst) == '#'))
    ++ofst;
  if (std::strncmp(line + ofst, "Frequency ", 10))
    return 0;
  /* string, matched; what is the frequency index ? */
  const int sz = std::strlen(line);
  const char* str = line + ofst + 11;
  auto res = std::from_chars(skipws(str), line + sz, num_freq);
  if (res.ec != std::errc {}) {
    // fprintf(stderr, "[ERROR] Failed resolving num freq from line %s (traceback: %s)\n", line, __func__);
    return 0;
  }
  str = res.ptr + 1;
  /* we must now find the ':' character */
  while (*str && (*str != ':'))
    ++str;
  if (*str != ':')
    return 0;
  /* and now the frequency in days */
  ++str;
  res = std::from_chars(skipws(str), line + sz, freq_days);
  if (res.ec != std::errc {}) {
    // fprintf(stderr, "[ERROR] Failed resolving freq from line %s (traceback: %s)\n", line, __func__);
    return 0;
  }
  /* good, we matched/resolved everyting! */
  return 1;
}

/* @brief Start index for CODE in a dpod_freq_corr line */
constexpr const int scode_start = 1;
/* @brief Start index for PT in a dpod_freq_corr line */
constexpr const int spt_start = 6;
/* @brief Start index for DOMES in a dpod_freq_corr line */
constexpr const int sdomes_start = 9;
/* @brief Start index for SOLN in a dpod_freq_corr line */
constexpr const int ssoln_start = 19;
/* @brief Start index for XYZ in a dpod_freq_corr line */
constexpr const int sxyz_start = 23;
/* @brief Start index for (float) data in a dpod_freq_corr line */
constexpr const int sdata_start = 27;
/**
 * #CODE PT __DOMES__SOLN_XYZ_COSAMP__COSSTD__SINAMP__SINSTD
 *  ADEA  A 91501S001  1   X   1.221   0.089  -1.066   0.088
 */
int resolve_psd_cor_data_line(const char* line, double* data) noexcept
{
  const char* str = line + 27;
  int error = 0;
  for (int i = 0; i < 4; i++) {
    auto res = std::from_chars(skipws(str), str + 8, data[i]);
    if (res.ec != std::errc {})
      ++error;
    str = res.ptr + 1;
  }
  return error;
}
} /* anonymous namespace */

int dso::parse_dpod_freq_corr(const char* fn,
    const std::vector<dso::sinex::SiteId>& sites_vec,
    std::vector<dso::SiteRealHarmonics>& harm) noexcept
{
  std::ifstream fin(fn);
  if (!fin.is_open()) {
    fprintf(stderr, "[ERROR] Failed opening dpod freq_corr file %s (traceback: %s)\n", fn, __func__);
    return 1;
  }

  /* pre-allocate vector capacity */
  harm.reserve(sites_vec.size());

  constexpr const int SZ = 256;
  char line[SZ];

  int nfreq;
  double freq = std::numeric_limits<double>::min();
  double data[4];

  int error = 0;
  while (fin.getline(line, SZ) && (!error)) {
    /* skip lines starting with '#', else parse */
    if (line[0] != '#') {
      /* first check site name */
      auto it = std::find_if(
          sites_vec.cbegin(), sites_vec.cend(), [&](const sinex::SiteId& site) {
            return !std::strncmp(site.site_code(), line + 1, 4) && !std::strncmp(site.point_code(), line + 6, 2);
          });
      /* the station is in the list */
      if (it != sites_vec.cend()) {

        error += resolve_psd_cor_data_line(line, data);
        if (!error) {
          /* do we already have a SiteRealHarmonics entry for the site */
          auto hit = std::find_if(
              harm.begin(), harm.end(), [&](const SiteRealHarmonics& sh) {
                return !std::strncmp(sh.site_name(), line + 1, 4);
              });

          /* just make sure we have indeed read a frequency */
          if (freq == std::numeric_limits<double>::min()) {
            fprintf(stderr, "[ERROR]. Failed reading frequency in dpod file %s (traceback: %s)\n", fn, __func__);
            return 9;
          }

          /* new Harmonics entry */
          if (hit == harm.end()) {
            harm.emplace_back(SiteRealHarmonics(line + 1, freq, data[2], data[0]));
          } else {
            hit->add_harmonic(freq, data[2], data[0]);
          }
        } /* no coefficients parsing error */
      } /* if station in list */
    } /* data line is not a comment */ else {
      is_new_frequency_line(line, nfreq, freq);
    }
  } /* reading through file entries */

  return 0;
}
