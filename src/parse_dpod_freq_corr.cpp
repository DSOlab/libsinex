#include "dpod.hpp"
#include "sinex.hpp"
#include <charconv>
#include <cstdio>
#include <fstream>
#include <stdexcept>

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
int resolve_freq_cor_data_line(const char *line, char &cmp,
                               double *data) noexcept {
  /* get component (one char) */
  cmp = line[24];
  if ((cmp != 'X' && cmp != 'Y') && (cmp != 'Z')) {
    fprintf(stderr,
            "[ERROR] Failed resolving dpod/freq line [%s], invalid XYZ field! "
            "(traceback: %s)\n",
            line, __func__);
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

std::vector<dso::Sinex::SiteCoordinateResults> dso::get_dpod_freq_corr(
    const char *fn, const dso::datetime<dso::nanoseconds> &t,
    const std::vector<dso::Sinex::SiteCoordinateResults> &sites_crd) {
  /* open dpod freq file */
  std::ifstream fin(fn);
  if (!fin.is_open()) {
    fprintf(stderr,
            "[ERROR] Failed opening dpod freq_corr file %s (traceback: %s)\n",
            fn, __func__);
    throw std::runtime_error("[ERROR] Failed opening dpod freq_corr file\n");
  }

  /* copy of input coordinates */
  std::vector<dso::Sinex::SiteCoordinateResults> scpy(sites_crd);

  /* zero-out (cartesian) positions */
  for (auto it = scpy.begin(); it != scpy.end(); ++it)
    it->x = it->y = it->z = 0e0;

  /* fractional day of year and phase at epoch */
  const auto ymd_ = t.as_ydoy();
  const int idoy_ = ymd_.dy().as_underlying_type();
  const double fdoy_ = t.fractional_days().days();
  const double fday = fdoy_ + idoy_;

  constexpr const int SZ = 128;
  char line[SZ];
  double cfreq = 0;
  double data[4];
  char ccmp = 'Q';
  double omega = 0e0;
  int nfreq = 0;

  int error = 0;
  while (fin.getline(line, SZ) && (!error)) {
    /* skip lines starting with '#', else parse */
    if (line[0] != '#') {
      /* find mathing site & SOLN_ID in scpy (if any) */
      auto it = std::find_if(
          scpy.begin(), scpy.end(),
          [&](const dso::Sinex::SiteCoordinateResults &site) {
            return ((!std::strncmp(site.msite.site_code(), line + 1,
                                   sinex::SITE_CODE_CHAR_SIZE) &&
                     !std::strncmp(site.msite.point_code(), line + 6,
                                   sinex::POINT_CODE_CHAR_SIZE)) &&
                    (!std::strncmp(site.msite.domes(), line + 9,
                                   sinex::DOMES_CHAR_SIZE) &&
                     !std::strncmp(site.soln_id(), line + 18,
                                   sinex::SOLN_ID_CHAR_SIZE)));
          });
      /* Here is a subtle point:
       * Sometimes, the SOLN_ID's in the dpod_freq file(s) do not exactly
       * match the ones given in the respective SINEX file (aka dpod*.snx).
       * For example, in the SINEX file it could be [  1 ] and in the freq_cor
       * as [   1]. So, if we matched nothing, let's check if we can find
       * a mathing site comparing the SOLN_ID fields as integers.
       */
      if (it == scpy.end()) {
        int sint = 2 * dso::sinex::NONINT_SOLN_ID;
        std::from_chars(skipws(line + 18), line + 18 + sinex::SOLN_ID_CHAR_SIZE,
                        sint);
        it = std::find_if(
            scpy.begin(), scpy.end(),
            [&](const dso::Sinex::SiteCoordinateResults &site) {
              return ((!std::strncmp(site.msite.site_code(), line + 1,
                                     sinex::SITE_CODE_CHAR_SIZE) &&
                       !std::strncmp(site.msite.point_code(), line + 6,
                                     sinex::POINT_CODE_CHAR_SIZE)) &&
                      (!std::strncmp(site.msite.domes(), line + 9,
                                     sinex::DOMES_CHAR_SIZE) &&
                       /*!std::strncmp(site.msolnid, line + 18,
                          sinex::SOLN_ID_CHAR_SIZE)*/
                       (site.soln_id_int() == sint)));
            });
      }

      /* the station is in the list */
      if (it != scpy.cend()) {

        error += resolve_freq_cor_data_line(line, ccmp, data);
        if (!error) {
          const double valmm =
              data[0] * std::cos(omega) + data[2] * std::sin(omega);
          switch (ccmp) {
          case 'X':
            it->x += valmm * 1e-3;
            break;
          case 'Y':
            it->y += valmm * 1e-3;
            break;
          case 'Z':
            it->z += valmm * 1e-3;
            break;
          default:
            fprintf(stderr,
                    "[ERROR] Invalid component in dpod_freq file %s; expected "
                    "one of X, Y or Z; line is %s (traceback: %s)\n",
                    fn, line, __func__);
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
  if (!((error == 0) && (nfreq >= 1))) {
    fprintf(stderr,
            "[ERROR] Failed applying parsing/applying frequency corrections "
            "from dpod file %s (traceback: %s)\n",
            fn, __func__);
    throw std::runtime_error(
        "[ERROR] Failed applying parsing/applying frequency corrections\n");
  }
  return scpy;
}

int dso::apply_dpod_freq_corr(
    const char *fn, const dso::datetime<dso::nanoseconds> &t,
    std::vector<dso::Sinex::SiteCoordinateResults> &sites_crd) noexcept {

  int error = 0;
  try {
    const auto cor = dso::get_dpod_freq_corr(fn, t, sites_crd);
    int idx = 0;
    for (auto it = sites_crd.begin(); it != sites_crd.end() && (!error);
         ++it, ++idx) {
      /* we assume here 1-to-1 correspondance between sites_crd and cor */
      auto itc = cor.cbegin() + idx;
      if ((std::strcmp(itc->msite.site_code(), it->msite.site_code()) ||
           std::strcmp(itc->msite.point_code(), it->msite.point_code())) ||
          (std::strcmp(itc->msite.domes(), it->msite.domes()) ||
           std::strcmp(itc->msolnid, it->msolnid))) {
        error += 1;
        fprintf(stderr,
                "[ERROR] Corrupt(?) parsing of dpod_freq_cor file; expected "
                "correction for %s and found %s (traceback: %s)\n",
                itc->msite.site_code(), it->msite.site_code(), __func__);
      }
      it->x += itc->x;
      it->y += itc->y;
      it->z += itc->z;
    }
  } catch (std::exception &e) {
    error = 1;
    fprintf(stderr,
            "[ERROR] Failed applying dpod frequency corrections from file %s "
            "(traceback: %s)\n",
            fn, __func__);
    fprintf(stderr, "[ERROR] Caught exception %s", e.what());
  }
  return error;
}
