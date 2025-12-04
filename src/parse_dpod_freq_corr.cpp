#include "dpod.hpp"
#include "sinex.hpp"
#include <charconv>
#include <cstdio>
#include <fstream>
#include <limits>

namespace {

/** @brief Trim left whitespaces.
 *
 * Returns the same string starting from the first non-whitespace character.
 */
const char *skipws(const char *str) noexcept {
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
int is_new_frequency_line(const char *line, int &num_freq,
                          double &freq_days) noexcept {
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
  const char *str = line + ofst + 11;
  auto res = std::from_chars(skipws(str), line + sz, num_freq);
  if (res.ec != std::errc{}) {
    // fprintf(stderr, "[ERROR] Failed resolving num freq from line %s
    // (traceback: %s)\n", line, __func__);
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
  if (res.ec != std::errc{}) {
    // fprintf(stderr, "[ERROR] Failed resolving freq from line %s (traceback:
    // %s)\n", line, __func__);
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
 * 012345678901234567890123456789012345678901234567890123456789
 *           10        20        30        40        50
 */
int resolve_freq_cor_data_line(const char *line, char cmp, double *data) noexcept {
  /* get component (one char) */
  cmp = line[24];
  if ((cmp != 'X' && cmp != 'Y') && (cmp != 'Z')) {
    fprintf(stderr, "[ERROR] Failed resolving dpod/freq line [%s], invalid XYZ field! (traceback: %s)\n", line, __func__);
    return 1;
  }
  const char *str = line + 27;
  int error = 0;
  for (int i = 0; i < 4; i++) {
    auto res = std::from_chars(skipws(str), str + 8, data[i]);
    if (res.ec != std::errc{})
      ++error;
    str = res.ptr + 1;
  }
  return error;
}
} /* anonymous namespace */

int dso::apply_dpod_freq_corr(
    const char *fn, 
    const dso::datetime<dso::nanoseconds> &t,
    const std::vector<dso::Sinex::SiteCoordinateResults> &sites_crd) noexcept {
  /* open dpod freq file */
  std::ifstream fin(fn);
  if (!fin.is_open()) {
    fprintf(stderr,
            "[ERROR] Failed opening dpod freq_corr file %s (traceback: %s)\n",
            fn, __func__);
    return 1;
  }

  /* copy of input coordinates */
  std::vector<dso::Sinex::SiteCoordinateResults> scpy(sites_crd);

  /* fractional day of year and phase at epoch */
  const auto ymd_ = t.as_ydoy();
  const int idoy_ = ymd_.dy().as_underlying_type();
  const double fdoy_ = t.fractional_days().days();
  const double fday = fdoy_ + idoy_;

  constexpr const int SZ = 128;
  char line[SZ];
  double cfreq = 0;
  double data[4];
  char ccmp;
  double omega = 0e0;
  int nfreq=0;

  int error = 0;
  while (fin.getline(line, SZ) && (!error)) {
    /* skip lines starting with '#', else parse */
    if (line[0] != '#') {
      /* first check site name */
      auto it = std::find_if(
          scpy.cbegin(), scpy.cend(), [&](const dso::Sinex::SiteCoordinateResults &site) {
            return ((!std::strncmp(site.msite.site_code(), line + 1, 4) &&
                   !std::strncmp(site.msite.point_code(), line + 6, 2)) &&
                   (!std::strncmp(site.msite.domes(), line + 9, 9) &&
                   !std::strncmp(site.msolnid, line + 18, 4)));
          });
      /* the station is in the list */
      if (it != scpy.cend()) {

        error += resolve_freq_cor_data_line(line, ccmp, data);
        if (!error) {
          const double valmm = data[0] * std::cos(omega) + data[2] * std::sin(omega);
          switch (ccmp) {
            case 'X':
              it->x += valmm*1e-3;
              break;
            case 'Y':
              it->y += valmm*1e-3;
              break;
            case 'Z':
              it->z += valmm*1e-3;
              break;
            default:
              fprintf(stderr, "[ERROR] Invalid component in dpod_freq file %s; expected one of X, Y or Z; line is %s (traceback: %s)\n", fn, line);
              error += 1;
          }
        } /* no coefficients parsing error */
      } /* if station in list */
    } else {
      /* maybe this is not a comment but a new frequency line */
      if (is_new_frequency_line(line, nfreq, cfreq)) {
        omega = 2e0 * M_PI * (fday / cfreq);
      }
    }
  } /* reading through file entries */

  /* we should have read at least one list of frequencies */
  return !((error==0) && (nfreq>=1));
}
