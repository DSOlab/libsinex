#ifndef __SINEX_FILE_PARSER_HPP__
#define __SINEX_FILE_PARSER_HPP__

#include "sinex_details.hpp"
#include <fstream>
#include <type_traits>
#include <vector>
#ifdef DEBUG
#include "datetime/datetime_write.hpp"
#include <cstdio>
#endif

namespace dso {

namespace sinex {
/* @class SinexBlockPosition A data block within a SINEX file */
struct SinexBlockPosition {
  std::ifstream::pos_type mpos; /* position from file begining */
  const char *mtype;            /* block description */
};                              /* SinexBlockPosition */

/* Enum class to hold SINEX Observation Codes.
 * Within SINEX files, this is a single character indicating the technique(s)
 * used to arrive at the solutions obtained in this SINEX file. It should be
 * consistent with the IERS convention.
 * This character code may be:
 * 'C'-Combined techniques used.
 * 'D'-DORIS,
 * 'L'-SLR,
 * 'M'-LLR,
 * 'P'-GNSS,
 * 'R'-VLBI
 */
enum class SinexObservationCode { COMBINED, DORIS, SLR, LLR, GNSS, VLBI };

/* Enum class to hold SINEX Constraint Codes.
 * Within SINEX files, this is a single character indicating the type of
 * constraints applied to a parameter. These can be:
 * 0-fixed/tight constraints,
 * 1-significant constraints,
 * 2-unconstrained.
 */
enum class SinexConstraintCode { FIXED, SIGNIFICANT, UNCONSTRAINED };

/* SinexObservationCode to char (may throw) */
char SinexObservationCode_to_char(SinexObservationCode);

/* char to SineObservationCode (may throw) */
SinexObservationCode char_to_SinexObservationCode(char c);

/* char to SinexConstraintCode (may throw) */
SinexConstraintCode char_to_SinexConstraintCode(char c);

/// @enum Choose a policy for comparing strings against parameter_type
enum class SiteMatchPolicyType { USEDOMES, IGNOREDOMES };

/// @class SiteMatchPolicy
/// @brief Dummy class to enable tag-dispatch for parameter_type_exists
///        function based on SiteMatchPolicyType
template <SiteMatchPolicyType T> struct SiteMatchPolicy : std::false_type {};

/// @class SiteMatchPolicy
/// @brief Dummy class to enable tag-dispatch for parameter_type_exists
///        function based on SiteMatchPolicyType. Specialization
///        for Strict string comparisson.
template <>
struct SiteMatchPolicy<SiteMatchPolicyType::USEDOMES> : std::true_type {};

/* Class to hold information stored (per line) in an SITE/ID Block */
class SiteId {
private:
  static constexpr const int site_code_at = 0;    /* [0,4] including NULL */
  static constexpr const int point_code_at = 5;   /* [5,7] including NULL */
  static constexpr const int domes_at = 8;        /* [8,17] including NULL */
  static constexpr const int description_at = 18; /* [18,39] including NULL */
  char charbuf__[40] = {'\0'};
  /* Observation Code: Observation technique(s) used [A1] */
  SinexObservationCode m_obscode;
  /* Approximate Longitude: Approximate longitude of the site in [rad] */
  double m_lon;
  /* Approximate Latitude: Approximate latitude of the site in [rad] */
  double m_lat;
  /* Approximate height of the site in [m] */
  double m_hgt;
  /* @brief Check if a given string matches the instance's SITE CODE.
   *
   * Only the first 4 characters of the input string are considered, and the
   * input string does not have to be null terminated.
   * @param[in] siteid String to check against the instance's SITE CODE
   * @return True if the first 4 characters of siteid are exactly the same as
   *         the instance's site_code.
   */
  bool sinex_site_equal_impl(const char *siteid,
                             std::false_type) const noexcept {
    return !std::strncmp(siteid, site_code(), 4);
  }
  /* @brief Check if a given string matches the instance's SITE CODE plus DOMES.
   *
   * The input string is assumed to be a string of type "CODE DOMES", where
   * CODE is the site's SITE CODE and DOMES is the site's DOMES NUMBER. The
   * two (sub)strings should be seperated by a whitespace character. The input
   * string does not have to be null-terminated.
   * Example: "DIOB 12602S012"
   * @param[in] siteid String to check against the instance's SITE CODE plus
   *             DOMES
   * @return True if both the siteid's SITE CODE and DOMES compare the same.
   */
  bool sinex_site_equal_impl(const char *siteid,
                             std::true_type) const noexcept {
    return (std::strlen(siteid) >= 14)
               ? ((!std::strncmp(siteid, site_code(), 4)) &&
                  (!std::strncmp(siteid + 5, domes(), 10)))
               : false;
  }

public:
  /* Site Code: Call sign for a site [A4] */
  char *site_code() noexcept { return charbuf__ + site_code_at; }
  const char *site_code() const noexcept { return charbuf__ + site_code_at; }
  /* Point Code: Physical monument used at a site [A2] */
  char *point_code() noexcept { return charbuf__ + point_code_at; }
  const char *point_code() const noexcept { return charbuf__ + point_code_at; }
  /* Unique Monument Identification: Unique alpha-nummeric monument
   * identification. For ITRF purposes, it is a nine character DOMES/DOMEX
   * number [A9]
   */
  char *domes() noexcept { return charbuf__ + domes_at; }
  const char *domes() const noexcept { return charbuf__ + domes_at; }
  /* Station Description: Free-format description of the site [A22] */
  char *description() noexcept { return charbuf__ + description_at; }
  const char *description() const noexcept {
    return charbuf__ + description_at;
  }
  /* @brief Get latitude in [rad] */
  double latitude() const noexcept { return m_lat; }
  double &latitude() noexcept { return m_lat; }
  /* @brief Get longitude in [rad] */
  double longitude() const noexcept { return m_lat; }
  double &longitude() noexcept { return m_lon; }
  /* @brief Get height in [m] */
  double height() const noexcept { return m_hgt; }
  double &height() noexcept { return m_hgt; }
  /* @brief Get observation code */
  sinex::SinexObservationCode obscode() const noexcept { return m_obscode; }
  sinex::SinexObservationCode &obscode() noexcept { return m_obscode; }

  /* @brief */
  template <SiteMatchPolicyType Policy = SiteMatchPolicyType::IGNOREDOMES>
  bool issame(const char *str) const noexcept {
    return sinex_site_equal_impl(str, SiteMatchPolicy<Policy>{});
  }
}; /* SiteId */

/* Class to hold information stored (per line) in a SITE/RECEIVER Block */
struct SiteReceiver {
  static constexpr const int site_code_at = 0;     /* [0,4] including NULL */
  static constexpr const int point_code_at = 5;    /* [5,7] including NULL */
  static constexpr const int soln_id_at = 8;       /* [8,12] including NULL */
  static constexpr const int rec_type_at = 13;     /* [13,33] including NULL */
  static constexpr const int rec_serial_at = 34;   /* [34,39] including NULL */
  static constexpr const int rec_firmware_at = 40; /* [40,51] including NULL */
  char charbuf__[64] = {'\0'};
  /* Site Code: Site code for which some parameters are estimated [A4]*/
  char *site_code() noexcept { return charbuf__ + site_code_at; }
  const char *site_code() const noexcept { return charbuf__ + site_code_at; }
  /* Point Code: Point Code at a site for which some parameters are estimated
   * [A2]
   */
  char *point_code() noexcept { return charbuf__ + point_code_at; }
  const char *point_code() const noexcept { return charbuf__ + point_code_at; }
  /* Solution ID: Solution Number at a Site/Point code for which some
   * parameters are estimated [A4]
   */
  char *soln_id() noexcept { return charbuf__ + soln_id_at; }
  const char *soln_id() const noexcept { return charbuf__ + soln_id_at; }
  /* Receiver Type: Receiver Name & model [A20] */
  char *rec_type() noexcept { return charbuf__ + rec_type_at; }
  const char *rec_type() const noexcept { return charbuf__ + rec_type_at; }
  /* Receiver Serial Number: Serial number of the receiver. Takes on value
   * '-----' if unknown. [A5]
   */
  char *rec_serial() noexcept { return charbuf__ + rec_serial_at; }
  const char *rec_serial() const noexcept { return charbuf__ + rec_serial_at; }
  /* Receiver Firmware: Firmware used by this receiver during the epoch
   * specified above. Takes on value '---------------' if unknown. [A11]
   */
  char *rec_firmware() noexcept { return charbuf__ + rec_firmware_at; }
  const char *rec_firmware() const noexcept {
    return charbuf__ + rec_firmware_at;
  }
  /* Time: Time since the receiver has been operating at the Site/Point. Value
   * 00:000:00000 indicates that the receiver has been operating at least
   * since the "File Epoch Start Time".
   */
  dso::datetime<dso::seconds> m_start{};
  /* Time: Time until the receiver is operated at a Site/Point. Value
   * 00:000:00000 indicates that the receiver has been operating at least
   * until the "File Epoch End Time"
   */
  dso::datetime<dso::seconds> m_stop{};
  /* Observation Code: Identification of the observation technique used [A1] */
  SinexObservationCode m_obscode;
}; /* SiteReceiver */

/* Class to hold information stored (per line) in a SITE/ANTENNA Block */
struct SiteAntenna {
  static constexpr const int site_code_at = 0;   /* [0,4] including NULL */
  static constexpr const int point_code_at = 5;  /* [5,7] including NULL */
  static constexpr const int soln_id_at = 8;     /* [8,12] including NULL */
  static constexpr const int ant_type_at = 13;   /* [13,33] including NULL */
  static constexpr const int ant_serial_at = 34; /* [34,39] including NULL */
  char charbuf__[40] = {'\0'};
  /* Site Code: Site code for which some parameters are estimated. [A4] */
  char *site_code() noexcept { return charbuf__ + site_code_at; }
  const char *site_code() const noexcept { return charbuf__ + site_code_at; }
  /* Point Code: Point Code at a site for which some parameters are estimated.
   * [A2]
   */
  char *point_code() noexcept { return charbuf__ + point_code_at; }
  const char *point_code() const noexcept { return charbuf__ + point_code_at; }
  /* Solution ID: Solution Number at a Site/Point code for which some
   * parameters are estimated. [A4]
   */
  char *soln_id() noexcept { return charbuf__ + soln_id_at; }
  const char *soln_id() const noexcept { return charbuf__ + soln_id_at; }
  /* Antenna Type: Antenna name & model. [A20] */
  char *ant_type() noexcept { return charbuf__ + ant_type_at; }
  const char *ant_type() const noexcept { return charbuf__ + ant_type_at; }
  /* Antenna Serial Number: Serial number of the antenna. Takes on value
   * '-----' if unknown. [A5]
   */
  char *ant_serial() noexcept { return charbuf__ + ant_serial_at; }
  const char *ant_serial() const noexcept { return charbuf__ + ant_serial_at; }
  /* Time: Time since the antenna has been installed at the Site/Point. Value
   * 00:000:00000 indicates that the antenna has been installed at least since
   * the "File Epoch Start Time".
   */
  dso::datetime<dso::seconds> m_start{};
  /* Time: Time until the antenna is installed at a Site/Point. Value
   * 00:000:00000 indicates that the antenna has been installed at least until
   * the "File Epoch End Time".
   */
  dso::datetime<dso::seconds> m_stop{};
  /* Observation Code: Identification of the observation technique used [A1] */
  SinexObservationCode m_obscode;
}; /* SiteAntenna */

/* class to hold a record line from block SOLUTION/ESTIMATE */
struct SolutionEstimate {
  static constexpr const int site_code_at = 0;  /* [0,4] including NULL */
  static constexpr const int point_code_at = 5; /* [5,7] including NULL */
  static constexpr const int soln_id_at = 8;    /* [8,12] including NULL */
  static constexpr const int units_at = 13;     /* [13,17] including NULL */
  char charbuf__[32] = {'\0'};
  /* Estimated Parameters Index: Index of estimated parameters. [I5] */
  int m_index;
  /* Parameter Type: Identification of the type of parameter. [A6]
   * This is a pointer to the respective type in the
   * dso::sinex::parameter_types[] array
   */
  const char *m_parameter_type;
  const char *parameter_type() const { return m_parameter_type; }
  /* Site Code: Site code for which some parameters are estimated. [A4] */
  char *site_code() noexcept { return charbuf__ + site_code_at; }
  const char *site_code() const noexcept { return charbuf__ + site_code_at; }
  /* Point Code: Point Code at a site for which some parameters are estimated.
   * [A2]
   */
  char *point_code() noexcept { return charbuf__ + point_code_at; }
  const char *point_code() const noexcept { return charbuf__ + point_code_at; }
  /* Solution ID: Solution Number at a Site/Point code for which some
   * parameters are estimated. [A4]
   */
  char *soln_id() noexcept { return charbuf__ + soln_id_at; }
  const char *soln_id() const noexcept { return charbuf__ + soln_id_at; }
  /* Parameter Units: Units used for the estimates and sigmas. [A4]
   * The notations are:
   * m (metres),
   * m/y (metres per year),
   * m/s2 (metres per second**2),
   * ppb (parts per billion),
   * ms (milliseconds),
   * msd2 (milliseconds per day**2),
   * mas (milli-arc-seconds),
   * ma/d (milli-arc-seconds / day),
   * rad (radians),
   * rd/y (radians per year),
   * rd/d (radians per day)
   */
  char *units() noexcept { return charbuf__ + units_at; }
  const char *units() const noexcept { return charbuf__ + units_at; }
  /* Constraint Code: Constraint applied to the parameter. [A1]*/
  SinexConstraintCode m_constraint;
  /* Parameter Estimate: Estimated value of the parameter. */
  double m_estimate;
  /* Parameter Standard Deviation: Estimated standard deviation for the
   * parameter.
   */
  double m_std_deviation;
  /* Time: Epoch at which the estimated parameter is valid. */
  dso::datetime<dso::seconds> m_epoch{};
}; /* SolutionEstimate */

/* class to hold a record line from block SOLUTION/EPOCH */
struct SolutionEpoch {
  static constexpr const int site_code_at = 0;  /* [0,4] including NULL */
  static constexpr const int point_code_at = 5; /* [5,7] including NULL */
  static constexpr const int soln_id_at = 8;    /* [8,12] including NULL */
  char charbuf__[16] = {'\0'};
  /* Site Code: Site code for which some parameters are estimated. [A4] */
  char *site_code() noexcept { return charbuf__ + site_code_at; }
  const char *site_code() const noexcept { return charbuf__ + site_code_at; }
  /* Point Code: Point Code at a site for which some parameters are estimated.
   * [A2]
   */
  char *point_code() noexcept { return charbuf__ + point_code_at; }
  const char *point_code() const noexcept { return charbuf__ + point_code_at; }
  /* Solution ID: Solution Number at a Site/Point code for which some
   * parameters are estimated. [A4]
   */
  char *soln_id() noexcept { return charbuf__ + soln_id_at; }
  const char *soln_id() const noexcept { return charbuf__ + soln_id_at; }
  /* Time: Time since the antenna has been installed at the Site/Point. Value
   * 00:000:00000 indicates that the antenna has been installed at least since
   * the "File Epoch Start Time".
   */
  dso::datetime<dso::seconds> m_start{};
  /* Time: Time until the antenna is installed at a Site/Point. Value
   * 00:000:00000 indicates that the antenna has been installed at least until
   * the "File Epoch End Time".
   */
  dso::datetime<dso::seconds> m_stop{};
  /* Time: Mean time of the observations for which the solution (SPNO) is
   * derived.
   */
  dso::datetime<dso::seconds> m_mean{};
  /* Observation Code: Identification of the observation technique used [A1] */
  SinexObservationCode m_obscode;
}; /* SolutionEstimate */

/* class to hold a record line from block SOLUTION/DATA_REJECT.
 * Such a block is NOT documened within the IERS (ie the standard format). It
 * is an extension used by e.g. the IDS to mark periods of time not included
 * in the combination. A SOLUTION/DATA_REJECT block is often included in the
 * dpod SINEX files.
 */
struct DataReject {
  static constexpr const int site_code_at = 0;  /* [0,4] including NULL */
  static constexpr const int point_code_at = 5; /* [5,7] including NULL */
  static constexpr const int soln_id_at = 8;    /* [8,12] including NULL */
  static constexpr const int colm_at = 13;      /* [13,13] including NULL */
  static constexpr const int cola_at = 14;      /* [14,14] including NULL */
  static constexpr const int comment_at = 15;   /* [15,63] including NULL */
  char charbuf__[64] = {'\0'};
  /* Site Code: Site code for which some parameters are estimated. [A4] */
  char *site_code() noexcept { return charbuf__ + site_code_at; }
  const char *site_code() const noexcept { return charbuf__ + site_code_at; }
  /* Point Code: Point Code at a site for which some parameters are estimated.
   * [A2]
   */
  char *point_code() noexcept { return charbuf__ + point_code_at; }
  const char *point_code() const noexcept { return charbuf__ + point_code_at; }
  /* Solution ID: Solution Number at a Site/Point code for which some
   * parameters are estimated. [A4]
   */
  char *soln_id() noexcept { return charbuf__ + soln_id_at; }
  const char *soln_id() const noexcept { return charbuf__ + soln_id_at; }
  /*  Column tagged 'M'
   */
  char colm() const noexcept { return charbuf__[colm_at]; }
  char &colm() noexcept { return charbuf__[colm_at]; }
  /*  Column tagged 'A'
   */
  char cola() const noexcept { return charbuf__[cola_at]; }
  char &cola() noexcept { return charbuf__[cola_at]; }
  /* Comment string */
  char *comment() noexcept { return charbuf__ + comment_at; }
  const char *comment() const noexcept { return charbuf__ + comment_at; }
  /* Time: start of rejection period */
  dso::datetime<dso::seconds> start;
  /* Time: end of rejection period */
  dso::datetime<dso::seconds> stop;
  /* Observation Code: Identification of the observation technique used [A1] */
  SinexObservationCode m_obscode;
};

/* Class to hold information stored (per line) in a SITE/ECCENTRICITY Block */
class SiteEccentricity {
private:
  /*
  *Code PT SOLN T Data_start__ Data_end____ AXE Up______ North___ East____
   ADEA  A    1 D 93:003:00000 98:084:11545 UNE   0.5100   0.0000   0.0000
  */
  static constexpr const int site_code_at = 0;   /* [0,4] including NULL */
  static constexpr const int point_code_at = 5;  /* [5,7] including NULL */
  static constexpr const int soln_id_at = 8;     /* [8,12] including NULL */
  static constexpr const int ref_system_at = 13; /* [13,16] including NULL */
  static constexpr const int ant_serial_at = 34; /* [34,39] including NULL */
  char charbuf__[40] = {'\0'};

  /* [Up or X,  North or Y, East or Z] offset from the marker to the Antenna
   * reference point (ARP). Units are [m].
   */
  double une[3] = {0e0, 0e0, 0e0};

public:
  /* Time: Time since the antenna has been installed at the Site/Point. Value
   * 00:000:00000 indicates that the antenna has been installed at least since
   * the "File Epoch Start Time".
   */
  dso::datetime<dso::seconds> start;
  /* Time: Time until the antenna is installed at a Site/Point. Value
   * 00:000:00000 indicates that the antenna has been installed at least until
   * the "File Epoch End Time".
   */
  dso::datetime<dso::seconds> stop;
  /* Observation Code: Identification of the observation technique used [A1] */
  SinexObservationCode m_obscode;

public:
  /* Site Code: Site code for which some parameters are estimated. [A4] */
  char *site_code() noexcept { return charbuf__ + site_code_at; }
  const char *site_code() const noexcept { return charbuf__ + site_code_at; }
  /* Point Code: Point Code at a site for which some parameters are estimated.
   * [A2]
   */
  char *point_code() noexcept { return charbuf__ + point_code_at; }
  const char *point_code() const noexcept { return charbuf__ + point_code_at; }
  /* Solution ID: Solution Number at a Site/Point code for which some
   * parameters are estimated. [A4]
   */
  char *soln_id() noexcept { return charbuf__ + soln_id_at; }
  const char *soln_id() const noexcept { return charbuf__ + soln_id_at; }
  /* Eccentricity Reference System: Reference system used to describe vector
   * distance from monument benchmark to the antenna reference point or
   * intersection of axis:
   * 'UNE' - Local reference system [Up, North, East].
   * 'XYZ' - Cartesian Reference System [X, Y, Z].
   * All units are in [m].
   */
  char *ref_system() noexcept { return charbuf__ + ref_system_at; }
  const char *ref_system() const noexcept { return charbuf__ + ref_system_at; }
  /* @brief Return an eccentricity component of choice; index in [0,3) */
  double eccentricity(int index) const noexcept { return une[index]; }
  double &eccentricity(int index) noexcept { return une[index]; }
}; /* SiteEccentricity */

/* @brief Parse a SINEX datetime string; accepted format is: "YY:DDD:SSSSS".
 * Time scale is "UTC",
 * YY = last 2 digits of the year,
 * if YY <= 50 implies 21-st century, if YY > 50 implies 20-th century,
 * DDD = 3-digit day in year,
 * SSSSS = 5-digit seconds in day.
 *
 * If the datetime string has the value of "00:000:00000", then t will get
 * the value of tdefault (usually implying, start/end of SINEX).
 *
 * @param[in] dtstr The datetime string. Can have any number of leading zeros
 * @param[in] tdefault Default t value in case the datetime string is
 *                     "00:000:00000"
 * @param[out] t       The datetime instance recorded in dtstr
 * @return Anything other than zero denotes an error
 */
int parse_sinex_date(const char *dtstr,
                     const dso::datetime<dso::seconds> &tdefault,
                     dso::datetime<dso::seconds> &t) noexcept;

} /* namespace sinex */

class Sinex {
private:
  using pos_t = std::ifstream::pos_type;

  /* SINEX filename */
  std::string m_filename;
  /* input stream (opened at c'tor) */
  std::ifstream m_stream;
  /* format version */
  float m_version;
  /* agency creating the file [A3] */
  char m_agency[4] = {'\0'};
  /* Identify the agency providing the data in the SINEX file [A3] */
  char m_data_agency[4] = {'\0'};
  /* Solution Contents */
  char m_sol_contents[6] = {'\0'};
  /* Creation time of this SINEX file */
  dso::datetime<dso::seconds> m_created_at;
  /* Start time of the data used in the SINEX solution */
  dso::datetime<dso::seconds> m_data_start;
  /* End time of the data used in the SINEX solution */
  dso::datetime<dso::seconds> m_data_stop;
  /* Technique(s) used to generate the SINEX solution */
  sinex::SinexObservationCode m_obscode;
  /* Single character indicating the constraint in the SINEX solution. */
  sinex::SinexConstraintCode m_constraint_code;
  /* Number of parameters estimated in this SINEX file */
  long m_num_estimates;
  /* Markers for easily accesing blocks. The entries here mark SINEX
   * block-positions and block-types. When placing the stream at a the
   * m_blocks[n], that means that:
   * We place the (input) stream poisition at m_blocks[n].mpos, i.e. the end
   * of the previous line, prior to the start of the new block (this means
   * that the next line to be read is the starting of a new block, e.g.
   * '+SOLUTION/EPOCHS'). Next line to be read is start of block described by
   * m_blocks[n].mtype.
   */
  std::vector<sinex::SinexBlockPosition> m_blocks;

  /* @brief Parse first SINEX line (header) and assign instance's member vars
   */
  int parse_first_line() noexcept;

  /* @brief Read the SINEX file through, and mark all positions of interest
   *       (i.e. start of blocks).
   * This function will fill in the m_blocks vector and perform a basic
   * sanity check of the SINEX file. Note that m_blocks does not contain
   * positions of the 'start of a block line', but rather positions of
   * 'eol before a new block line'.
   * This function should only be called once, at the instance's ctor.
   */
  int mark_blocks() noexcept;

  /* @brief Go (i.e. place the stream) at the the start of a block in a SINEX
   *        instance.
   * Asserts that the mark_blocks() function has already been called. Example:
   * if (goto_block("SOLUTION/EPOCHS")) return 1;
   * Now, next line to be read is: "+SOLUTION/EPOCHS"
   *
   * @param[in] A valid SINEX block (see e.g. dso::sinex::block_names[]);
   *            expects a NULL terminated C-string.
   */
  int goto_block(const char *block) noexcept;

  /* @brief Given a block name, find the relevant entry in the m_blocks
   *        vector.
   * @param[in] blk A valid SINEX block name (C-string), e.g. "SOLUTION/EPOCHS"
   * @return An iterator to the relevant entry in m_blocks (or m_blocks.cend()
   *         if no such block exists in the SINEX file).
   */
  std::vector<sinex::SinexBlockPosition>::const_iterator
  find_block(const char *blk) noexcept {
    return std::find_if(m_blocks.cbegin(), m_blocks.cend(),
                        [&](const sinex::SinexBlockPosition &sbp) {
                          return !std::strcmp(sbp.mtype, blk);
                        });
  }

public:
  /* return the SINEX filename */
  std::string filename() const noexcept { return m_filename; }

  /* @brief Parse the SITE/ID block of the SINEX file and collect info for
   *        given sites
   * This function will search through the SITE/ID block, and collect all
   * infor for the sites that are included in the sites vector. Matching of
   * stations can be parformed in two ways:
   * 1. if use_domes is set to false, then only the SITE CODE is checked, i.e.
   *    each string in the sites vector should contain a 4-char id of the
   *    station (of interest). E.g. "DIOA", "HERS", etc ...
   * 2. if use_domes is set to true, then both the SITE CODE and the DOMES are
   *    checked. This means that the input sites vector should contain strings
   *    that include the SITE CODE a whitespace charatcer and the DOMES
   *    identifier. E.g. "DIOB 12602S012", "HOFC 10204S001", etc ...
   * Only the sites that are matched will be included in the (output) site_vec
   * vector.
   * @param[in] sites Vector of stations of interest; strings of the form:
   *            e.g. "DIOB" or "DIOB 12602S012" (if we are matching DOMES).
   *            The strings do not have to be null-terminated.
   * @param[in] use_domes Mark if we are also matching DOMES numbers (except
   *            SITE CODEs).
   * @param[out] site_vec A vector containing one SiteId entry for each of the
   *            the sites that were matched (i.e. it could be that
   *            size(sites) != size(site_vec)
   * @return Anything other than 0 denotes an error
   */
  int parse_block_site_id(const std::vector<const char *> &sites,
                          bool use_domes,
                          std::vector<sinex::SiteId> &site_vec) noexcept;

  /* @brief Parse the (whole) SITE/ID block of the SINEX and return all info
   * @param[out] site_vec A vector containing one SiteId entry for each of the
   *            the sites that are included in the block.
   * @return Anything other than 0 denotes an error
   */
  int parse_block_site_id(std::vector<sinex::SiteId> &site_vec) noexcept {
    return parse_block_site_id(std::vector<const char *>(), false, site_vec);
  }

  /* @brief Parse the whole SITE/RECEIVER Block off from the SINEX instance.
   * @param[inout] site_vec A vector of sinex::SiteReceiver instances, one
   *               entry for each block line.
   * @return Anything other than zero denotes an error
   */
  int parse_block_site_receiver(
      std::vector<sinex::SiteReceiver> &site_vec) noexcept;

  int parse_block_site_antenna(
      std::vector<sinex::SiteAntenna> &site_vec) noexcept;

  /* @brief Parse the whole SOLUTION/ESTIMATE Block off from the SINEX
   * instance and collect sinex::SolutionEstimate records for the SITES of
   * interest. The sites of interest are the ones included in the sites_vec
   * (input) vector. Any SOLUTION/ESTIMATE line for which we have a matching
   * SITE ID and POINT ID will be collected.
   * @param[inout] site_vec A vector of sinex::SolutionEstimate instances, one
   *               entry for each block line.
   * @param[in] sites_vec A vector of sinex::SiteId instances to match against,
   *               using the SITE CODE and POINT CODE fields.
   * @return Anything other than zero denotes an error
   */
  int parse_block_solution_estimate(
      const std::vector<sinex::SiteId> &sites_vec,
      std::vector<sinex::SolutionEstimate> &estimates_vec) noexcept;

  /* @brief
   * Parse the whole SOLUTION/DATA_REJECT Block off from the SINEX instance
   * and collect sinex::DataReject instances for the SITES of interest. The
   * sites of interest are the ones included in the sites_vec (input) vector.
   * Any SOLUTION/DATA_REJECT line for which we have a matching SITE ID and
   * POINT ID will be collected, so long as it falls within or overlaps the
   * given interval [from, to].
   * @param[in] sites_vec A vector of sinex::SiteId instances to match against,
   *             using the SITE CODE and POINT CODE fields.
   * @param[out] site_vec A vector of sinex::DataReject instances, one
   *             entry for each block line (note that this means that for one
   *             station we can have multiple rejection intervals).
   * @param[in] from Start of period of interest. Rejection intervals that end
   *             before this date will not be collected (inclusive).
   * @to[in]    Stop period of interest. Rejection intervals that start after
   *             this date will not be considered (inclusive).
   * @return Anything other than zero denotes an error
   */
  int parse_block_data_reject(const std::vector<sinex::SiteId> &site_vec,
                              std::vector<sinex::DataReject> &out_vec,
                              const dso::datetime<dso::seconds> from =
                                  dso::datetime<dso::seconds>::min(),
                              const dso::datetime<dso::seconds> to =
                                  dso::datetime<dso::seconds>::max()) noexcept;

  /* @brief Read and parse the SITE/ECCENTRICITY block off from the SINEX
   * instance.
   *
   * @param[in] site_vec A list of SITE/ID instances that shall be considered.
   *            We will be matching records according to SITE CODE and
   *            POINT CODE.
   * @param[in] t  The time at which we want the eccentricities. If later
   *            than the instance's DATA STOP time, then we will be assuming
   *            that the validity intervals that end at DATA STOP time are
   *            valid internaly in the future.Hence, if DATA STOP = 2022/365
   *            and t = 2023/001 and we encounter a record with data
   *            stop = "00:000:00000", then it is presumed that this record is
   *            valid for the given t.
   * @param[out] out_vec A vector of sinex::SiteEccentricity for some or all
   *            of the sites contained in site_vec, valid for the time given
   *            (ie t)
   * @return Anything other than 0 denotes an error
   */
  int parse_block_site_eccentricity(
      const std::vector<sinex::SiteId> &site_vec,
      const dso::datetime<dso::seconds> &t,
      std::vector<sinex::SiteEccentricity> &out_vec) noexcept;

  /* @brief Parse the SINEX block SOLUTION/EPOCHS and return a vector of
   *        sinex::SolutionEpoch instances for the sites included in site_vec,
   *        valid at the epoch t.
   *        The sites of interest are the ones included in the sites_vec
   *        (input) vector. Any SOLUTION/EPOCHS line for which we have a
   *        matching SITE ID and POINT ID will be inspected. If the input time
   *        t matches the SOLUTION/EPOCHS recorded interval, then the record
   *        will be collcted and returned in the out_vec.
   * @param[in] t Epoch for which we want the solution record (SOLUTION/EPOCHS
   *              line).
   * @param[in] site_vec A list of SITE/ID instances that shall be considered.
   *              We will be matching records according to SITE CODE and
   *              POINT CODE.
   * @param[out] out_vec A vector of sinex::SolutionEpoch instances for some
   *              or all of the sites contained in site_vec, valid for the
   *              time given (ie t)
   * @return Anything other than zero denotes an error
   */
  int parse_solution_epoch(
      const std::vector<sinex::SiteId> &site_vec,
      const dso::datetime<dso::seconds> &t,
      std::vector<dso::sinex::SolutionEpoch> &out_vec) noexcept;

  int get_solution_estimate(const char *site_codes[],
                            const dso::datetime<dso::seconds> &t,
                            bool error_if_missing = false) noexcept;

  /* @brief Constructor (may throw). This will:
   * 1. Assign filename,
   * 2. open the stream,
   * 3. parse_first_line() to assign member vars,
   * 4. call mark_blocks() to fill in m_blocks
   */
  Sinex(const char *fn);
  Sinex(const Sinex &) = delete;
  Sinex &operator=(const Sinex &) = delete;
  ~Sinex() noexcept {
    if (m_stream.is_open())
      m_stream.close();
  }

}; /* Sinex */

int filter_solution_estimates(
    const std::vector<sinex::SolutionEstimate> &estimates,
    const dso::datetime<dso::seconds> &t,
    std::vector<sinex::SolutionEstimate> &filtered_estimates) noexcept;
int filter_solution_estimates(
    const std::vector<dso::sinex::SolutionEstimate> &estimates,
    const std::vector<dso::sinex::SolutionEpoch> &epochs,
    std::vector<dso::sinex::SolutionEstimate> &filtered_estimates) noexcept;

} /* namespace dso */

#endif /*__SINEX_FILE_PARSER_HPP__*/
