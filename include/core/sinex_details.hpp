/** @file
 * A list of commonly used parameters, constants and functions commonly used
 * to interact with SINEX file. Most of the defined parameters & functions are
 * wrapped around the dso::sinex namespace; they should act in the
 * 'background', i.e. should be hidden from the public API not exposed to the
 * end-user.
 */

#ifndef __SINEX_FILE_DETAILS_HPP__
#define __SINEX_FILE_DETAILS_HPP__

#include "datetime/calendar.hpp"
#include <algorithm>
#include <cstring>

namespace dso::sinex {

/** @brief Max characters in a SINEX line */
constexpr int max_sinex_chars = 128;

/** @brief Number of characters in a DOMES (no  null-terminating char 
 *         included), see
 * https://itrf.ign.fr/en/network/domes/description
 */
constexpr const int max_domes_chars = 9;

/** @brief Blocks allowed in a SINEX file */
const char *const block_names[] = {
    "FILE/REFERENCE", "FILE/COMMENT", "INPUT/HISTORY", "INPUT/FILES",
    "INPUT/ACKNOWLEDGEMENTS", "NUTATION/DATA", "PRECESSION/DATA", "SOURCE/ID",
    "SITE/ID", "SITE/DATA", "SITE/RECEIVER", "SITE/ANTENNA",
    "SITE/GPS_PHASE_CENTER", "SITE/GAL_PHASE_CENTER", "SITE/ECCENTRICITY",
    "SATELLITE/ID", "SATELLITE/PHASE_CENTER", "BIAS/EPOCHS", "SOLUTION/EPOCHS",
    "SOLUTION/STATISTICS", "SOLUTION/ESTIMATE", "SOLUTION/APRIORI",
    "SOLUTION/MATRIX_ESTIMATE L CORR", "SOLUTION/MATRIX_ESTIMATE L COVA",
    "SOLUTION/MATRIX_ESTIMATE L INFO", "SOLUTION/MATRIX_ESTIMATE U CORR",
    "SOLUTION/MATRIX_ESTIMATE U COVA", "SOLUTION/MATRIX_ESTIMATE U INFO",
    "SOLUTION/MATRIX_APRIORI L CORR", "SOLUTION/MATRIX_APRIORI L COVA",
    "SOLUTION/MATRIX_APRIORI L INFO", "SOLUTION/MATRIX_APRIORI U CORR",
    "SOLUTION/MATRIX_APRIORI U COVA", "SOLUTION/MATRIX_APRIORI U INFO",
    "SOLUTION/NORMAL_EQUATION_VECTOR", "SOLUTION/NORMAL_EQUATION_MATRIX L",
    "SOLUTION/NORMAL_EQUATION_MATRIX U",
    // ---------------------------------------------------------------------//
    //  The following are not defined in [1] but are present in IDS Sinex   //
    //  files for DPOD reference frame definition.                          //
    //  see https://ids-doris.org/combination/dpod.html                     //
    //----------------------------------------------------------------------//
    "SOLUTION/DISCONTINUITY", "SOLUTION/DATA_REJECT", "STATION/TO_BE_UPDATED"};

/** @brief Number of different blocks in block_names */
constexpr int block_names_size = sizeof(block_names) / sizeof(char *);

/** @brief Parameter types allowed in SINEX files */
const char *const parameter_types[] = {
    "STAX",   /* station X coordinate, m */
    "STAY",   /* station Y coordinate, m */
    "STAZ",   /* station Z coordinate, m */
    "VELX",   /* station X velocity, m/y */
    "VELY",   /* station Y velocity, m/y */
    "VELZ",   /* station Z velocity, m/y */
    "XGC",    /* geocenter X coordinate, m */
    "YGC",    /* geocenter Y coordinate, m */
    "ZGC",    /* geocenter Z coordinate, m */
    "RS_RA",  /* radio source right ascension, rad */
    "RS_DE",  /* radio source declin., rad */
    "RS_RAR", /* radio source right ascension rate, rad/y */
    "RS_DER", /* radio source declination rate, rad/y */
    "RS_PL",  /* radio source parallax, rad */
    "LOD",    /* length of day, ms */
    "UT",     /* delta time UT1-UTC, ms */
    "XPO",    /* X polar motion, mas */
    "YPO",    /* Y polar motion, mas */
    "XPOR",   /* X polar motion rate, mas/d */
    "YPOR",   /* Y polar motion rate, mas/d */
    "NUT_LN", /* nutation correction in longitude, mas */
    "NUT_OB", /* nutation correction in obliquity, mas */
    "NUTRLN", /* nutation rate in longitude, mas/d */
    "NUTROB", /* nutation rate in obliquity, mas/d */
    "NUT_X",  /* nutation correction X, mas */
    "NUT_Y",  /* nutation correction Y, mas */
    "NUTR_X", /* nutation rate in X mas/d */
    "NUTR_Y", /* nutation rate in Y mas/d */
    "SAT__X", /* Satellite X coord., m */
    "SAT__Y", /* Satellite Y coord., m */
    "SAT__Z", /* Satellite Z coord., m */
    "SAT_VX", /* Satellite X velocity, m/s */
    "SAT_VY", /* Satellite Y velocity, m/s */
    "SAT_VZ", /* Satellite Z velocity, m/s */
    "SAT_RP", /* Radiation pressure, */
    "SAT_GX", /* GX scale, */
    "SAT_GZ", /* GZ scale, */
    "SATYBI", /* GY bias, m/s2 */
    "TROTOT", /* wet + dry Trop. delay, m */
    "TRODRY", /* dry Trop. delay, m */
    "TROWET", /* wet Trop. delay, m */
    "TGNTOT", /* troposphere gradient in north (wet + dry), m */
    "TGNWET", /* troposphere gradient in north (only wet), m */
    "TGNDRY", /* troposphere gradient in north (only dry), m */
    "TGETOT", /* troposphere gradient in east (wet + dry), m */
    "TGEWET", /* troposphere gradient in east (only wet), m */
    "TGEDRY", /* troposphere gradient in east (only dry), m */
    "RBIAS",  /* range bias, m */
    "TBIAS",  /* time bias, ms */
    "SBIAS",  /* scale bias, ppb */
    "ZBIAS",  /* troposphere bias at zenith, m */
    "AXI_OF", /* VLBI antenna axis offset, m */
    "SATA_Z", /* sat. antenna Z offset m */
    "SATA_X", /* sat. antenna X offset, m */
    "SATA_Y", /* sat. antenna Y offset, m */
    "CN",     /* spherical harmonic coefficient C_nm */
    "SN",     /* spherical harmonic coefficient S_nm */
    // ---------------------------------------------------------------------//
    //  The following are not defined in [1] but are present in IERS Sinex  //
    //  files for describing PSD models                                     //
    //----------------------------------------------------------------------//
    "AEXP_N",
    "AEXP_E",
    "AEXP_U",
    "TEXP_N",
    "TEXP_E",
    "TEXP_U",
    "ALOG_N",
    "ALOG_E",
    "ALOG_U",
    "TLOG_N",
    "TLOG_E",
    "TLOG_U",
};

/** @brief Number of different parameter types in parameter_types */
constexpr int parameter_types_size = sizeof(parameter_types) / sizeof(char *);

namespace details {

/** @brief Match a given string to any string in parameter_types array
 *
 * @param[in] ptype String to match (does not have to be null-terminated).
 * @param[out] index The index of the parameter (in the parameter_types array)
 *                   that was matched. If no parameter was matched, it will be
 *                   set to -1
 * @return True if ptype matches any parameter in the parameter_types array.
 *         False otherwise.
 *
 * @note This implementation will only consider the first n characters of
 *   ptype, where n is the number of characters of the string it is compared
 *   against. ptype can have as many characters beyond n, and they will not be
 *   considered; e.g.
 *   ptype = "STAX" will match "STAX", but so will "STAXX", "STAX " and
 *   "STAXfoobar"
 */
inline bool parameter_type_exists_impl(const char *ptype, int &index,
                                       std::false_type) noexcept {
  index = -1;
  auto it =
      std::find_if(parameter_types, parameter_types + parameter_types_size,
                   [&](const char *const str) {
                     return !std::strncmp(ptype, str, std::strlen(str));
                   });
  if (it == parameter_types + parameter_types_size)
    return false;

  index = std::distance(parameter_types, it);
  return true;
}

/** Copy src to dest, ommiting leading whitespaces (if any)
 *
 * Copies count chars from the object pointed to by src to the object pointed 
 * to by dest. Note that we start counting from the first char of src, NOT the
 * first non-whitespace character.
 * This function actually calls std::memcpy, hence if the objects overlap, the 
 * behavior is undefined.
 * If either dest or src is an invalid or null pointer, the behavior is 
 * undefined, even if count is zero. 
 *
 * @param[in] dest  pointer to the memory location to copy to 
 * @param[in] src   pointer to the memory location to copy from 
 * @param[in] count number of bytes to copy 
 * @return pointer to dest
 */
inline const char *ltrim_cpy(char *__restrict__ dest,
                             const char *__restrict__ src,
                             int count) noexcept {
  const char *c = src;
  while (*c && *c == ' ')
    ++c;
  std::memcpy(dest, c, count - (c - src));
  return dest;
}

/** @brief Match a given string to any string in parameter_types array
 *
 * @param[in] ptype String to match (null-terminiated)
 * @param[out] index The index of the parameter (in the parameter_types array)
 *                   that was matched. If no parameter was matched, it will be
 *                   set to -1
 * @return True if ptype matches any parameter in the parameter_types array.
 *         False otherwise.
 *
 * @note This implementation will only match ptype to a parameter_type if and
 *   only if they are completely the same (up untill the null terminating
 *   character). E.g. ptype = "STAX" will match "STAX" but "STAXX",
 *   "STAX " and "STAXfoobar" will not matched.
 */
inline bool parameter_type_exists_impl(const char *ptype, int &index,
                                       std::true_type) noexcept {
  index = -1;
  auto it = std::find_if(
      parameter_types, parameter_types + parameter_types_size,
      [&](const char *const str) { return !std::strcmp(ptype, str); });
  if (it == parameter_types + parameter_types_size)
    return false;
  index = std::distance(parameter_types, it);
  return true;
}

/** @brief Choose a policy for comparing strings against parameter_type */
enum class ParameterMatchPolicyType { Strict, NonStrict };

/** @class ParameterMatchPolicy
 * Dummy class to enable tag-dispatch for parameter_type_exists function
 * based on ParameterMatchPolicyType
 */
template <ParameterMatchPolicyType T>
struct ParameterMatchPolicy : std::false_type {};

/** @class ParameterMatchPolicy
 * Dummy class to enable tag-dispatch for parameter_type_exists function based
 * on ParameterMatchPolicyType. Specialization for Strict string comparisson.
 */
template <>
struct ParameterMatchPolicy<ParameterMatchPolicyType::Strict> : std::true_type {
};

/* @enum Choose a policy for matching sites (see dso::sinex::SiteId::issame */
enum class SiteMatchPolicyType { USEDOMES, IGNOREDOMES };

/** @class SiteMatchPolicy
 * Dummy class to enable tag-dispatch for functions comparing site names,
 * based on SiteMatchPolicyType
 */
template <SiteMatchPolicyType T> struct SiteMatchPolicy : std::false_type {};

/** @class SiteMatchPolicy
 * Dummy class to enable tag-dispatch for functions comparing site names,
 * based on SiteMatchPolicyType. Specialization for USEDOMES string
 * comparisson.
 */
template <>
struct SiteMatchPolicy<SiteMatchPolicyType::USEDOMES> : std::true_type {};

} /* namespace details */

/** @brief Match a given string to any string in parameter_types array
 *
 * @param[in] ptype String to match
 * @tparam Policy Choose between Strict (aka the input string but be an exact
 *         match up to the null-terminating character to the compared-against
 *         string), or NonStrict (aka only the the first n characters of the
 *         input string are considered, where n is the length of each string
 *         in the parameter_types array).
 * @example
 * ptype = "STAX" will match "STAX" regardless of Policy.
 * ptype = "STAXfoobar" will match "STAX" only if Policy is NonStrict
 */
template <details::ParameterMatchPolicyType Policy =
              details::ParameterMatchPolicyType::Strict>
bool parameter_type_exists(const char *ptype, int &index) noexcept {
  return parameter_type_exists_impl(ptype, index,
                                    details::ParameterMatchPolicy<Policy>{});
}

} /* namespace dso::sinex */

#endif
