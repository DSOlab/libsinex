#ifndef __SINEX_FILE_PARSER_HPP__
#define __SINEX_FILE_PARSER_HPP__

#include "ggdatetime/dtcalendar.hpp"
#include <algorithm>
#include <fstream>
#include <vector>
#ifdef DEBUG
#include "ggdatetime/datetime_write.hpp"
#include "ggeodesy/units.hpp"
#endif

namespace sinex {
constexpr int max_sinex_chars = 128;
constexpr ngpt::datetime<ngpt::seconds> missing_sinex_date =
    ngpt::datetime<ngpt::seconds>{ngpt::year(0), ngpt::day_of_year(0),
                                  ngpt::seconds(0L)};

const char *const block_names[] = {
    "FILE/REFERENCE",
    "FILE/COMMENT",
    "INPUT/HISTORY",
    "INPUT/FILES",
    "INPUT/ACKNOWLEDGEMENTS",
    "NUTATION/DATA",
    "PRECESSION/DATA",
    "SOURCE/ID",
    "SITE/ID",
    "SITE/DATA",
    "SITE/RECEIVER",
    "SITE/ANTENNA",
    "SITE/GPS_PHASE_CENTER",
    "SITE/GAL_PHASE_CENTER",
    "SITE/ECCENTRICITY",
    "SATELLITE/ID",
    "SATELLITE/PHASE_CENTER",
    "BIAS/EPOCHS",
    "SOLUTION/EPOCHS",
    "SOLUTION/STATISTICS",
    "SOLUTION/ESTIMATE",
    "SOLUTION/APRIORI",
    "SOLUTION/MATRIX_ESTIMATE L CORR",
    "SOLUTION/MATRIX_ESTIMATE L COVA",
    "SOLUTION/MATRIX_ESTIMATE L INFO",
    "SOLUTION/MATRIX_ESTIMATE U CORR",
    "SOLUTION/MATRIX_ESTIMATE U COVA",
    "SOLUTION/MATRIX_ESTIMATE U INFO",
    "SOLUTION/MATRIX_APRIORI L CORR",
    "SOLUTION/MATRIX_APRIORI L COVA",
    "SOLUTION/MATRIX_APRIORI L INFO",
    "SOLUTION/MATRIX_APRIORI U CORR",
    "SOLUTION/MATRIX_APRIORI U COVA",
    "SOLUTION/MATRIX_APRIORI U INFO",
    "SOLUTION/NORMAL_EQUATION_VECTOR",
    "SOLUTION/NORMAL_EQUATION_MATRIX L",
    "SOLUTION/NORMAL_EQUATION_MATRIX U",
};
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
    "SN"      // spherical harmonic coefficient S_nm
    /* 
      -------------------------------------------------------------------------
      The following are not defined in [1] but are present in IERS Sinex files
      for describing PSD models 
      -------------------------------------------------------------------------
    */
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

/// @brief Match a given string to any string in parameter_types array
/// @param[in] ptype String to match
/// @note This implementation will only consider the first n
///       characters of ptype, where n is the number of characters of
///       the string it is compared againse. ptype can have as many
///       characters beyond n, and they will not be considered; e.g.
///       ptype = "STAX" will match "STAX", but so will "STAXX",
///       "STAX " and "STAXfoobar".
bool parameter_type_exists_impl(const char *ptype, std::false_type) noexcept {
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
bool parameter_type_exists_impl(const char *ptype, std::true_type) noexcept {
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

struct SinexBlockPosition {
  std::ifstream::pos_type mpos;
  const char *mtype;
};

struct SiteId {
  char m_site_code[5] = {'\0'};
  char m_point_code[3] = {'\0'};
  char m_domes[10] = {'\0'};
  char m_obs_code;
  char m_description[23] = {'\0'};
  double m_lon, m_lat; // radians
  double m_hgt;        // height in m
#ifdef DEBUG
  void print() const noexcept {
    printf("Site Code    %s\n", m_site_code);
    printf("Point Code   %s\n", m_point_code);
    printf("Domes Number %s\n", m_domes);
    printf("Obs. Code    %c\n", m_obs_code);
    printf("Description  %s\n", m_description);
    printf("lat/lon/hgt %+7.1f/%+7.1f/%7.2f\n", ngpt::rad2deg(m_lon),
           ngpt::rad2deg(m_lat), m_hgt);
  }
#endif
};

struct SiteReceiver {
  char m_site_code[5] = {'\0'};
  char m_point_code[3] = {'\0'};
  char m_soln_id[5] = {'\0'};
  char m_obs_code;
  ngpt::datetime<ngpt::seconds> m_start{}, m_stop{};
  char m_rec_type[21] = {'\0'};
  char m_rec_serial[6] = {'\0'};
  char m_rec_firmware[12] = {'\0'};
#ifdef DEBUG
  void print() const noexcept {
    printf("Site Code    %s\n", m_site_code);
    printf("Point Code   %s\n", m_point_code);
    printf("Solution Id  %s\n", m_soln_id);
    printf("Obs. Code    %c\n", m_obs_code);
    printf("Rec. Type    %s\n", m_rec_type);
    printf("Rec. Serial  %s\n", m_rec_serial);
    printf("Rec. Firmw   %s\n", m_rec_firmware);
    printf("From %s to %s\n", ngpt::strftime_ymd_hmfs(m_start).c_str(),
           ngpt::strftime_ymd_hmfs(m_stop).c_str());
  }
#endif
};
struct SiteAntenna {
  char m_site_code[5] = {'\0'};
  char m_point_code[3] = {'\0'};
  char m_soln_id[5] = {'\0'};
  char m_obs_code;
  ngpt::datetime<ngpt::seconds> m_start{}, m_stop{};
  char m_ant_type[21] = {'\0'};
  char m_ant_serial[6] = {'\0'};
#ifdef DEBUG
  void print() const noexcept {
    printf("Site Code    %s\n", m_site_code);
    printf("Point Code   %s\n", m_point_code);
    printf("Solution Id  %s\n", m_soln_id);
    printf("Obs. Code    %c\n", m_obs_code);
    printf("Ant. Type    %s\n", m_ant_type);
    printf("Ant. Serial  %s\n", m_ant_serial);
    printf("From %s to %s\n", ngpt::strftime_ymd_hmfs(m_start).c_str(),
           ngpt::strftime_ymd_hmfs(m_stop).c_str());
  }
#endif
};
struct SolutionEstimate {
  int m_index;
  char m_param_type[7] = {'\0'};
  char m_site_code[5] = {'\0'};
  char m_point_code[3] = {'\0'};
  char m_soln_id[5] = {'\0'};
  ngpt::datetime<ngpt::seconds> m_epoch{};
  char m_units[5] = {'\0'};
  char m_constraint_code = '\0';
  double m_estimate, m_std_deviation;
};
} // namespace sinex

class Sinex {
private:
  using pos_t = std::ifstream::pos_type;

  std::string m_filename;
  std::ifstream m_stream;
  float m_version;
  char m_agency[4] = {'\0'};
  ngpt::datetime<ngpt::seconds> m_created_at, m_data_start, m_data_stop;
  char m_data_agency[4] = {'\0'};
  char m_obs_code, m_constraint_code;
  char m_sol_contents[6] = {'\0'};
  int m_num_estimates;
  std::vector<sinex::SinexBlockPosition> m_blocks;

  std::vector<sinex::SinexBlockPosition>::const_iterator
  find_block(const char *blk) noexcept {
    return std::find_if(m_blocks.cbegin(), m_blocks.cend(),
                        [&](const sinex::SinexBlockPosition &sbp) {
                          return !std::strcmp(sbp.mtype, blk);
                        });
  }

  int goto_block(const char *block) noexcept;

public:
  int parse_first_line() noexcept;
  int mark_blocks() noexcept;

  // parse blocks
  int parse_block_site_id(std::vector<sinex::SiteId> &site_vec) noexcept;
  int parse_block_site_receiver(
      std::vector<sinex::SiteReceiver> &site_vec) noexcept;
  int parse_block_site_antenna(
      std::vector<sinex::SiteAntenna> &site_vec) noexcept;
  int parse_block_solution_estimate(
      std::vector<sinex::SolutionEstimate> &site_vec) noexcept;

  Sinex(const char *fn);
  Sinex(const Sinex &) = delete;
  Sinex &operator=(const Sinex &) = delete;

#ifdef DEBUG
  void print_members() const;
  void print_blocks() const noexcept;
#endif

}; // Sinex

#endif //__SINEX_FILE_PARSER_HPP__