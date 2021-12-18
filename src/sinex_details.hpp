#ifndef __SINEX_FILE_DETAILS_HPP__
#define __SINEX_FILE_DETAILS_HPP__

#include "ggdatetime/dtcalendar.hpp"
#include <algorithm>
#include <cstring>

namespace dso::sinex {

constexpr int max_sinex_chars = 128;

constexpr dso::datetime<dso::seconds> missing_sinex_date =
    dso::datetime<dso::seconds>{dso::year(0), dso::day_of_year(0),
                                dso::seconds(0L)};

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
constexpr int block_names_size = sizeof(block_names) / sizeof(char *);

const char *const parameter_types[] = {
    "STAX",   // station X coordinate, m
    "STAY",   // station Y coordinate, m
    "STAZ",   // station Z coordinate, m
    "VELX",   // station X velocity, m/y
    "VELY",   // station Y velocity, m/y
    "VELZ",   // station Z velocity, m/y
    "XGC",    // geocenter X coordinate, m
    "YGC",    // geocenter Y coordinate, m
    "ZGC",    // geocenter Z coordinate, m
    "RS_RA",  // radio source right ascension, rad
    "RS_DE",  // radio source declin., rad
    "RS_RAR", // radio source right ascension rate, rad/y
    "RS_DER", // radio source declination rate, rad/y
    "RS_PL",  // radio source parallax, rad
    "LOD",    // length of day, ms
    "UT",     // delta time UT1-UTC, ms
    "XPO",    // X polar motion, mas
    "YPO",    // Y polar motion, mas
    "XPOR",   // X polar motion rate, mas/d
    "YPOR",   // Y polar motion rate, mas/d
    "NUT_LN", // nutation correction in longitude, mas
    "NUT_OB", // nutation correction in obliquity, mas
    "NUTRLN", // nutation rate in longitude, mas/d
    "NUTROB", // nutation rate in obliquity, mas/d
    "NUT_X",  // nutation correction X, mas
    "NUT_Y",  // nutation correction Y, mas
    "NUTR_X", // nutation rate in X mas/d
    "NUTR_Y", // nutation rate in Y mas/d
    "SAT__X", // Satellite X coord., m
    "SAT__Y", // Satellite Y coord., m
    "SAT__Z", // Satellite Z coord., m
    "SAT_VX", // Satellite X velocity, m/s
    "SAT_VY", // Satellite Y velocity, m/s
    "SAT_VZ", // Satellite Z velocity, m/s
    "SAT_RP", // Radiation pressure,
    "SAT_GX", // GX scale,
    "SAT_GZ", // GZ scale,
    "SATYBI", // GY bias, m/s2
    "TROTOT", // wet + dry Trop. delay, m
    "TRODRY", // dry Trop. delay, m
    "TROWET", // wet Trop. delay, m
    "TGNTOT", // troposphere gradient in north (wet + dry), m
    "TGNWET", // troposphere gradient in north (only wet), m
    "TGNDRY", // troposphere gradient in north (only dry), m
    "TGETOT", // troposphere gradient in east (wet + dry), m
    "TGEWET", // troposphere gradient in east (only wet), m
    "TGEDRY", // troposphere gradient in east (only dry), m
    "RBIAS",  // range bias, m
    "TBIAS",  // time bias, ms
    "SBIAS",  // scale bias, ppb
    "ZBIAS",  // troposphere bias at zenith, m
    "AXI_OF", // VLBI antenna axis offset, m
    "SATA_Z", // sat. antenna Z offset m
    "SATA_X", // sat. antenna X offset, m
    "SATA_Y", // sat. antenna Y offset, m
    "CN",     // spherical harmonic coefficient C_nm
    "SN",     // spherical harmonic coefficient S_nm
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
constexpr int parameter_types_size = sizeof(parameter_types) / sizeof(char *);

/// @brief Resolve a string of type YY:DOY:SECOD to a datetime instance
/// @param[in] str A string of type: YY:DDD:SECOD, where yy is the year, DDD is
///            the day of year and SECOD the seconds of day. The string can
///            have any number of whitespace characters at the begining. After
///            the SECOD field, the string must have a non-numeric character.
/// @return The datetime instance represented by the input string, in resolution
///         dso::seconds.
dso::datetime<dso::seconds> parse_snx_date(const char *str);

/// @brief Match a given string to any string in parameter_types array
/// @param[in] ptype String to match
/// @note This implementation will only consider the first n
///       characters of ptype, where n is the number of characters of
///       the string it is compared againse. ptype can have as many
///       characters beyond n, and they will not be considered; e.g.
///       ptype = "STAX" will match "STAX", but so will "STAXX",
///       "STAX " and "STAXfoobar".
inline bool parameter_type_exists_impl(const char *ptype,
                                       std::false_type) noexcept {
  auto it =
      std::find_if(parameter_types, parameter_types + parameter_types_size,
                   [&](const char *const str) {
                     return !std::strncmp(ptype, str, std::strlen(str));
                   });
  if (it == parameter_types + parameter_types_size)
    return false;
  return true;
}

/// @brief Match a given string to any string in parameter_types array
/// @param[in] ptype String to match
/// @note This implementation will only match ptype to a parameter_type
///       if and only if they are completely the same (up untill the
///       null terminating character). E.g. ptype = "STAX" will match
///       "STAX" but "STAXX", "STAX " and "STAXfoobar" will not match
inline bool parameter_type_exists_impl(const char *ptype,
                                       std::true_type) noexcept {
  auto it = std::find_if(
      parameter_types, parameter_types + parameter_types_size,
      [&](const char *const str) { return !std::strcmp(ptype, str); });
  if (it == parameter_types + parameter_types_size)
    return false;
  return true;
}

/// @enum Choose a policy for comparing strings against parameter_type
enum class ParameterMatchPolicyType { Strict, NonStrict };

/// @class ParameterMatchPolicy
/// @brief Dummy class to enable tag-dispatch for parameter_type_exists
///        function based on ParameterMatchPolicyType
template <ParameterMatchPolicyType T>
struct ParameterMatchPolicy : std::false_type {};

/// @class ParameterMatchPolicy
/// @brief Dummy class to enable tag-dispatch for parameter_type_exists
///        function based on ParameterMatchPolicyType. Specialization
///        for Strict string comparisson.
template <>
struct ParameterMatchPolicy<ParameterMatchPolicyType::Strict> : std::true_type {
};

/// @brief Match a given string to any string in parameter_types array
/// @param[in] ptype String to match
/// @tparam Policy Choose between Strict (aka the input string but be
///         an exact match up to the null-terminating character to
///         the compared-against string), or NonStrict (aka only the
///         the first n characters of the input string are considered,
///         where n is the length of each string in the parameter_types
///         array).
/// @example
/// ptype = "STAX" will match "STAX" regardless of Policy.
/// ptype = "STAXfoobar" will match "STAX" only if Policy is
/// NonStrict
template <ParameterMatchPolicyType Policy = ParameterMatchPolicyType::Strict>
bool parameter_type_exists(const char *ptype) noexcept {
  return parameter_type_exists_impl(ptype, ParameterMatchPolicy<Policy>{});
}

/// @brief Skip whitespace characters
/// Give a string str and a maximum number of characters to check (aka
/// max_count), return a pointer to the first non-whitespace character in str.
/// Only the first max_count characters are considered, hence if we do not find
/// a non-whitespace character after max_count chars, nullptr is returned.
/// @param[in] str Geiven string to skip whitespaces chars for
/// @param[in] max_count Max number of characters to consider
/// @param[out] chars_skipped Number of characters skipped untill we reached a
///            non-whitespace char
/// @return A pointer to the first non-whitespace character in the region
///         str[0:max_count); if no such character exists, nullptr is returned
const char *bypass_wsc(const char *str, int max_count,
                       int &chars_skipped) noexcept;

/// @brief Copy characters from src to dest until  we either reach a whitespace
/// character or max_count characters are copied
/// @param[in] dest Destination string (copy to)
/// @param[in] src Source string (copy from)
/// @param[in] max_count Maximum number of characters to copy (if src contains
///            a whitespace character before max_count, then the copy will stop
///            at this char)
/// @return Number of characters copied
int copy_untill_wsc(char *dest, const char *src, int max_count) noexcept;
} // namespace dso::sinex

#endif
