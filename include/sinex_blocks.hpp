/** @file
 * A list of classes to describe and hold information recorded in SINEX blocks.
 * Each class should describe one block, and in essence it should hold all
 * information that such a block line holds.
 * For example the class dso::sinex::SiteId should hold one individual record
 * line of the SINEX block SITE/ID.
 *
 * References:
 * [1] SINEX - Solution (Software/technique) INdependent EXchange Format
 * Version 2.02 (December 01, 2006)
 */

#ifndef __SINEX_FILE_BLOCK_CLASSES_HPP__
#define __SINEX_FILE_BLOCK_CLASSES_HPP__

#include "core/sinex_details.hpp"
#include <fstream>
#include <type_traits>
#include <vector>

namespace dso {

namespace sinex {
/** @class SinexBlockPosition A data block within a SINEX file */
struct SinexBlockPosition {
  std::ifstream::pos_type mpos; /* position from file begining */
  const char *mtype;            /* block description */
}; /* SinexBlockPosition */

/** Enum class to hold SINEX Observation Codes.
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

/** Enum class to hold SINEX Constraint Codes.
 * Within SINEX files, this is a single character indicating the type of
 * constraints applied to a parameter. These can be:
 * 0-fixed/tight constraints,
 * 1-significant constraints,
 * 2-unconstrained.
 */
enum class SinexConstraintCode { FIXED, SIGNIFICANT, UNCONSTRAINED };

/** @brief SinexObservationCode to char (may throw) */
char SinexObservationCode_to_char(SinexObservationCode);

/** @brief char to SineObservationCode (may throw) */
SinexObservationCode char_to_SinexObservationCode(char c);

/** @brief char to SinexConstraintCode (may throw) */
SinexConstraintCode char_to_SinexConstraintCode(char c);

/** @brief Size (in chars) in various, commonly used fields NOT including 
 * null-terminating character 
 */
constexpr const int SITE_CODE_CHAR_SIZE = 4;
constexpr const int POINT_CODE_CHAR_SIZE = 2;
constexpr const int DOMES_CHAR_SIZE = 9;
constexpr const int SOLN_ID_CHAR_SIZE = 4;
constexpr const int PARAMETER_TYPE_CHAR_SIZE = 6;

/** @brief If a SOLN_ID cannot be transformed to an int, this is what gets 
 * returned
 */
constexpr const int NONINT_SOLN_ID = -999;

/** @class Hold information stored (per line) in an SITE/ID Block 
 *
 * Example snippet:
+SITE/ID
*CODE PT __DOMES__ T _STATION DESCRIPTION__ APPROX_LON_ APPROX_LAT_ _APP_H_
 ADEA  A 91501S001 D TERRE ADELIE, ANTARCTI 140 00 05.2 -66 39 45.6     1.8
 ADEB  A 91501S002 D TERRE ADELIE, ANTARCTI 140 00 07.3 -66 39 54.7    -0.0
 *
 * 
 * see https://ivscc.gsfc.nasa.gov/products-data/sinex_v202.pdf
 *
 * |_________________S_I_T_E___I_D___D_A_T_A___L_I_N_E_________________|
 * |                |                                   |              |
 * |__Field_________|______Description__________________|___Format_____|
 * |                |                                   |              |
 * | [Site Code]    | Call sign for a site.             | 1X,A4        |
 * |________________|___________________________________|______________|
 * |                |                                   |              |
 * | [Point Code]   | Physical monument used at a site  | 1X,A2        |
 * |________________|___________________________________|______________|
 * |                |                                   |              |
 * | Unique Monument| Unique alpha-nummeric monument    | 1X,A9        |
 * | Identification | identification. For ITRF purposes,|              |
 * |                | it is a nine character DOMES/DOMEX|              |
 * |                | number (five/six digits, followed |              |
 * |                | by the single letter 'M' or 'S',  |              |
 * |                | followed by four/three digits)    |              |
 * |________________|___________________________________|______________|
 * |                |                                   |              |
 * | [Observation   | Observation technique(s) used.    | 1X,A1        |
 * | Code]          |                                   |              |
 * |________________|___________________________________|______________|
 * |                |                                   |              |
 * | Station        | Free-format description of the    | 1X,A22       |
 * | Description    | site, typically the town and/or   |              |
 * |                | country.                          |              |
 * |________________|___________________________________|______________|
 * |                |                                   |              |
 * | Approximate    | Approximate longitude of the site | 1X,I3,       |
 * | Longitude      | in degrees(E/+), minutes and      | 1X,I2,       |
 * |                | seconds.                          | 1X,F4.1      |
 * |________________|___________________________________|______________|
 * |                |                                   |              |
 * | Approximate    | Approximate latitude of the site  | 1X,I3,       |
 * | Latitude       | in degrees(NS/+-), minutes and    | 1X,I2,       |
 * |                | seconds.                          | 1X,F4.1      |
 * |________________|___________________________________|______________|
 * |                |                                   |              |
 * | Approximate    | Approximate height of the site in | 1X,F7.1      |
 * | Height         | metres.                           |              |
 * |________________|___________________________________|______________|
 *                                                      |              |
 *                                                      | 75           |
 *                                                      |______________|
 *
 */
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

  /** @brief Check if a given string matches the instance's SITE CODE.
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

  /** @brief Check if a given string matches the instance's SITE CODE plus
   *        DOMES.
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
                  (!std::strncmp(siteid + 5, domes(), 9)))
               : false;
  }

public:
  /** Site Code: Call sign for a site [A4] */
  char *site_code() noexcept { return charbuf__ + site_code_at; }
  const char *site_code() const noexcept { return charbuf__ + site_code_at; }

  /** Point Code: Physical monument used at a site [A2] */
  char *point_code() noexcept { return charbuf__ + point_code_at; }
  const char *point_code() const noexcept { return charbuf__ + point_code_at; }

  /** Unique Monument Identification: Unique alpha-nummeric monument
   * identification. For ITRF purposes, it is a nine character DOMES/DOMEX
   * number [A9]
   */
  char *domes() noexcept { return charbuf__ + domes_at; }
  const char *domes() const noexcept { return charbuf__ + domes_at; }

  /** Station Description: Free-format description of the site [A22] */
  char *description() noexcept { return charbuf__ + description_at; }
  const char *description() const noexcept {
    return charbuf__ + description_at;
  }

  /** @brief Get latitude in [rad] */
  double latitude() const noexcept { return m_lat; }
  double &latitude() noexcept { return m_lat; }

  /** @brief Get longitude in [rad] */
  double longitude() const noexcept { return m_lon; }
  double &longitude() noexcept { return m_lon; }

  /** @brief Get height in [m] */
  double height() const noexcept { return m_hgt; }
  double &height() noexcept { return m_hgt; }

  /** @brief Get observation code */
  sinex::SinexObservationCode obscode() const noexcept { return m_obscode; }
  sinex::SinexObservationCode &obscode() noexcept { return m_obscode; }

  /** @brief Convert to null terminated C-string, according to [1] */
  const char *to_str(char *buf) const noexcept;

  /** @brief Match a given site to this instance's site
   * @param[in] str Given site (as string) to match against. Does not have to
   *         be null-terminated.
   * @tparam Policy Matching; should we consider DOMES number or not?
   * @return True if the given site (string) matches the one in this instance,
   *         false otherwise.
   */
  template <details::SiteMatchPolicyType Policy =
                details::SiteMatchPolicyType::IGNOREDOMES>
  bool issame(const char *str) const noexcept {
    return sinex_site_equal_impl(str, details::SiteMatchPolicy<Policy>{});
  }
}; /* SiteId */

/** @class Hold information stored (per line) in a SITE/RECEIVER Block */
struct SiteReceiver {
  static constexpr const int site_code_at = 0;     /* [0,4] including NULL */
  static constexpr const int point_code_at = 5;    /* [5,7] including NULL */
  static constexpr const int soln_id_at = 8;       /* [8,12] including NULL */
  static constexpr const int rec_type_at = 13;     /* [13,33] including NULL */
  static constexpr const int rec_serial_at = 34;   /* [34,39] including NULL */
  static constexpr const int rec_firmware_at = 40; /* [40,51] including NULL */
  char charbuf__[64] = {'\0'};

  /** Site Code: Site code for which some parameters are estimated [A4]*/
  char *site_code() noexcept { return charbuf__ + site_code_at; }
  const char *site_code() const noexcept { return charbuf__ + site_code_at; }

  /** Point Code: Point Code at a site for which some parameters are estimated
   * [A2]
   */
  char *point_code() noexcept { return charbuf__ + point_code_at; }
  const char *point_code() const noexcept { return charbuf__ + point_code_at; }

  /** Solution ID: Solution Number at a Site/Point code for which some
   * parameters are estimated [A4]
   */
  char *soln_id() noexcept { return charbuf__ + soln_id_at; }
  const char *soln_id() const noexcept { return charbuf__ + soln_id_at; }
  int soln_id_int() const noexcept;

  /** Receiver Type: Receiver Name & model [A20] */
  char *rec_type() noexcept { return charbuf__ + rec_type_at; }
  const char *rec_type() const noexcept { return charbuf__ + rec_type_at; }

  /** Receiver Serial Number: Serial number of the receiver. Takes on value
   * '-----' if unknown. [A5]
   */
  char *rec_serial() noexcept { return charbuf__ + rec_serial_at; }
  const char *rec_serial() const noexcept { return charbuf__ + rec_serial_at; }

  /** Receiver Firmware: Firmware used by this receiver during the epoch
   * specified above. Takes on value '---------------' if unknown. [A11]
   */
  char *rec_firmware() noexcept { return charbuf__ + rec_firmware_at; }
  const char *rec_firmware() const noexcept {
    return charbuf__ + rec_firmware_at;
  }

  /** Time: Time since the receiver has been operating at the Site/Point. Value
   * 00:000:00000 indicates that the receiver has been operating at least
   * since the "File Epoch Start Time".
   */
  dso::datetime<dso::nanoseconds> m_start{};

  /** Time: Time until the receiver is operated at a Site/Point. Value
   * 00:000:00000 indicates that the receiver has been operating at least
   * until the "File Epoch End Time"
   */
  dso::datetime<dso::nanoseconds> m_stop{};

  /** Observation Code: Identification of the observation technique used [A1] */
  SinexObservationCode m_obscode;
}; /* SiteReceiver */

/** @class Hold information stored (per line) in a SITE/ANTENNA Block */
struct SiteAntenna {
  static constexpr const int site_code_at = 0;   /* [0,4] including NULL */
  static constexpr const int point_code_at = 5;  /* [5,7] including NULL */
  static constexpr const int soln_id_at = 8;     /* [8,12] including NULL */
  static constexpr const int ant_type_at = 13;   /* [13,33] including NULL */
  static constexpr const int ant_serial_at = 34; /* [34,39] including NULL */
  char charbuf__[40] = {'\0'};

  /** Site Code: Site code for which some parameters are estimated. [A4] */
  char *site_code() noexcept { return charbuf__ + site_code_at; }
  const char *site_code() const noexcept { return charbuf__ + site_code_at; }

  /** Point Code: Point Code at a site for which some parameters are estimated.
   * [A2]
   */
  char *point_code() noexcept { return charbuf__ + point_code_at; }
  const char *point_code() const noexcept { return charbuf__ + point_code_at; }

  /** Solution ID: Solution Number at a Site/Point code for which some
   * parameters are estimated. [A4]
   */
  char *soln_id() noexcept { return charbuf__ + soln_id_at; }
  const char *soln_id() const noexcept { return charbuf__ + soln_id_at; }
  int soln_id_int() const noexcept;

  /** Antenna Type: Antenna name & model. [A20] */
  char *ant_type() noexcept { return charbuf__ + ant_type_at; }
  const char *ant_type() const noexcept { return charbuf__ + ant_type_at; }

  /** Antenna Serial Number: Serial number of the antenna. Takes on value
   * '-----' if unknown. [A5]
   */
  char *ant_serial() noexcept { return charbuf__ + ant_serial_at; }
  const char *ant_serial() const noexcept { return charbuf__ + ant_serial_at; }

  /** Time: Time since the antenna has been installed at the Site/Point. Value
   * 00:000:00000 indicates that the antenna has been installed at least since
   * the "File Epoch Start Time".
   */
  dso::datetime<dso::nanoseconds> m_start{};

  /** Time: Time until the antenna is installed at a Site/Point. Value
   * 00:000:00000 indicates that the antenna has been installed at least until
   * the "File Epoch End Time".
   */
  dso::datetime<dso::nanoseconds> m_stop{};

  /** Observation Code: Identification of the observation technique used [A1] */
  SinexObservationCode m_obscode;
}; /* SiteAntenna */

/** @class Hold a record line from block SOLUTION/ESTIMATE 
 *
 * see https://ivscc.gsfc.nasa.gov/products-data/sinex_v202.pdf
 *
|______S_O_L_U_T_I_O_N___E_S_T_I_M_A_T_E___D_A_T_A___L_I_N_E________|
|                |                                   |              |
|__Field_________|______Description__________________|___Format_____|
|                |                                   |              |
| Estimated Para-| Index of estimated parameters.    | 1X,I5        |
| meters Index   | values from 1 to the number of    |              |
|                | parameters.                       |              |
|________________|___________________________________|______________|
|                |                                   |              |
|[Parameter Type]| Identification of the type of     | 1X,A6        | 
|                | parameter.                        |              |
|________________|___________________________________|______________|
|                |                                   |              |
| [Site Code]    | Site code for which the           | 1X,A4        |
|                | parameter is estimated.           |              |
|________________|___________________________________|______________|
|                |                                   |              |
| [Point Code]   | Point Code for which the          | 1X,A2        |
|                | parameter is estimated.           |              |
|________________|___________________________________|______________|
|                |                                   |              |
| [Solution ID]  | Solution ID at a Site/Point       | 1X,A4        |
|                | code for which the parameter      |              |
|                | is estimated.                     |              |
|________________|___________________________________|______________|
|                |                                   |              |
| [Time]         | Epoch at which the estimated      | 1X,I2.2,     |
|                | parameter is valid.               | 1H:,I3.3,    |
|                | For bias parameters the beginning | 1H:,I5.5     |
|                | of the pass (identical to the     |              |
|                | BIAS/EPOCHS block).               |              |
|________________|___________________________________|______________|
|                |                                   |              |
| Parameter Units| Units used for the estimates and  | 1X,A4        |
|                | sigmas. The notations are:        |              |
|                | m (metres),                       |              |
|                | m/y (metres per year),            |              |
|                | m/s2 (metres per second**2),      |              |
|                | ppb (parts per billion),          |              |
|                | ms (milliseconds),                |              |
|                | msd2 (milliseconds per day**2),   |              |
|                | mas (milli-arc-seconds),          |              |
|                | ma/d (milli-arc-seconds / day),   |              |
|                | rad (radians),                    |              |
|                | rd/y (radians per year),          |              |
|                | rd/d (radians per day).           |              |
|________________|___________________________________|______________|
|                |                                   |              |
| [Constraint    | Constraint applied to the parame- | 1X,A1        |
| Code]          | ter.                              |              |
|________________|___________________________________|______________|
|                |                                   |              |
| Parameter      | Estimated value of the parameter. | 1X,E21.15    |
| Estimate       |                                   |              |
|________________|___________________________________|______________|
|                |                                   |              |
| Parameter      | Estimated standard deviation for  | 1X,E11.6     |
| Standard       | the parameter.                    |              |
| Deviation      |                                   |              |
|________________|___________________________________|______________|
                                                     |              |
                                                     | 80           |
                                                     |______________|
 *
 */
class SolutionEstimate {
  static constexpr const int site_code_at = 0;  /* [0,4] including NULL */
  static constexpr const int point_code_at = 5; /* [5,7] including NULL */
  static constexpr const int soln_id_at = 8;    /* [8,12] including NULL */
  static constexpr const int units_at = 13;     /* [13,17] including NULL */
  char charbuf__[32] = {'\0'};
  const char *m_parameter_type;

  /** Estimated Parameters Index: Index of estimated parameters. [I5] */
  int m_index;

  /** Constraint Code: Constraint applied to the parameter. [A1]*/
  SinexConstraintCode m_constraint;

  /** Parameter Estimate: Estimated value of the parameter. */
  double m_estimate;

  /** Parameter Standard Deviation: Estimated standard deviation for the
   * parameter.
   */
  double m_std_deviation;

  /** Time: Epoch at which the estimated parameter is valid. */
  dso::datetime<dso::nanoseconds> m_epoch{};

public:
  /** Parameter Type: Identification of the type of parameter. [A6]
   * This is a pointer to the respective type in the
   * dso::sinex::parameter_types[] array
   */
  const char *parameter_type() const noexcept { return m_parameter_type; }
  void set_parameter_type(const char *s) noexcept { m_parameter_type = s; }

  /** @brief Get constraint code */
  SinexConstraintCode constraint() const noexcept { return m_constraint; }
  SinexConstraintCode &constraint() noexcept { return m_constraint; }

  /** Site Code: Site code for which some parameters are estimated. [A4] */
  char *site_code() noexcept { return charbuf__ + site_code_at; }
  const char *site_code() const noexcept { return charbuf__ + site_code_at; }

  /** Point Code: Point Code at a site for which some parameters are estimated.
   * [A2]
   */
  char *point_code() noexcept { return charbuf__ + point_code_at; }
  const char *point_code() const noexcept { return charbuf__ + point_code_at; }

  /** Solution ID: Solution Number at a Site/Point code for which some
   * parameters are estimated. [A4]
   */
  char *soln_id() noexcept { return charbuf__ + soln_id_at; }
  const char *soln_id() const noexcept { return charbuf__ + soln_id_at; }
  int soln_id_int() const noexcept;

  /** Parameter Units: Units used for the estimates and sigmas. [A4]
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

  /** @brief Return the estimated value */
  double estimate() const noexcept { return m_estimate; }
  double &estimate() noexcept { return m_estimate; }

  /** @brief Return the std. deviation of the estimate */
  double std_deviation() const noexcept { return m_std_deviation; }
  double &std_deviation() noexcept { return m_std_deviation; }

  /** @brief Return the estimation epoch */
  dso::datetime<dso::nanoseconds> epoch() const noexcept { return m_epoch; }
  dso::datetime<dso::nanoseconds> &epoch() noexcept { return m_epoch; }

  /** @brief Return the index */
  int index() const noexcept { return m_index; }
  int &index() noexcept { return m_index; }
}; /* SolutionEstimate */

/** @class Hold a record line from block SOLUTION/EPOCH 
 *
 *
|____S_O_L_U_T_I_O_N___E_P_O_C_H_S___D_A_T_A___L_I_N_E______________|
|                |                                   |              |
|__Field_________|______Description__________________|___Format_____|
|                |                                   |              |
| [Site Code]    | Site code for which some          | 1X,A4        |
|                | parameters are estimated.         |              |
|________________|___________________________________|______________|
|                |                                   |              |
| [Point Code]   | Point Code at a site for which    | 1X,A2        |
|                | some parameters are estimated.    |              |
|________________|___________________________________|______________|
|                |                                   |              |
| [Solution ID]  | Solution Number at a Site/Point   | 1X,A4        |
|                | code for which some parameters    |              |
|                | are estimated.                    |              |
|________________|___________________________________|______________|
|                |                                   |              |
| [Observation   | Identification of the observation | 1X,A1        |
| Code]          | technique used.                   |              |
|________________|___________________________________|______________|
|                |                                   |              |
| [Time]         | Start time for which the solution | 1X,I2.2,     |
|                | identified (SPNO) has observations| 1H:,I3.3,    |
|                |                                   | 1H:,I5.5     |
|________________|___________________________________|______________|
|                |                                   |              |
| [Time]         | End time for which the solution   | 1X,I2.2,     |
|                | identified (SPNO) has observations| 1H:,I3.3,    |
|                |                                   | 1H:,I5.5     |
|________________|___________________________________|______________|
|                |                                   |              |
| [Time]         | Mean time of the observations for | 1X,I2.2,     |
|                | which the solution (SPNO) is      | 1H:,I3.3,    |
|                | derived.                          | 1H:,I5.5     |
|________________|___________________________________|______________|
                                                     |              |
                                                     | 54           |
                                                     |______________|

 *
 * */
struct SolutionEpoch {
  static constexpr const int site_code_at = 0;  /* [0,4] including NULL */
  static constexpr const int point_code_at = 5; /* [5,7] including NULL */
  static constexpr const int soln_id_at = 8;    /* [8,12] including NULL */
  char charbuf__[16] = {'\0'};

  /** Site Code: Site code for which some parameters are estimated. [A4] */
  char *site_code() noexcept { return charbuf__ + site_code_at; }
  const char *site_code() const noexcept { return charbuf__ + site_code_at; }

  /** Point Code: Point Code at a site for which some parameters are estimated.
   * [A2]
   */
  char *point_code() noexcept { return charbuf__ + point_code_at; }
  const char *point_code() const noexcept { return charbuf__ + point_code_at; }

  /** Solution ID: Solution Number at a Site/Point code for which some
   * parameters are estimated. [A4]
   */
  char *soln_id() noexcept { return charbuf__ + soln_id_at; }
  const char *soln_id() const noexcept { return charbuf__ + soln_id_at; }
  int soln_id_int() const noexcept;

  /** Time: Start time for which the solution identified (SPNO) has
   * observations
   */
  dso::datetime<dso::nanoseconds> m_start{};

  /** Time: End time for which the solution identified (SPNO) has
   * observations
   */
  dso::datetime<dso::nanoseconds> m_stop{};

  /** Time: Mean time of the observations for which the solution (SPNO) is
   * derived.
   */
  dso::datetime<dso::nanoseconds> m_mean{};

  /** Observation Code: Identification of the observation technique used [A1] */
  SinexObservationCode m_obscode;

  /** @brief Check if two SolutionEpoch instances describe the same site
   * A site is consdered a match, if both the SITE CODE and the POINT CODE are
   * exactly the same.
   * @return True if the sites are exact match; false otherwise.
   */
  bool match_site(const SolutionEpoch &se) const noexcept {
    return (!std::strcmp(site_code(), se.site_code())) &&
           (!std::strcmp(point_code(), se.point_code()));
  }
}; /* SolutionEpoch */

/** @class Hold a record line from block SOLUTION/DATA_REJECT.
 *
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

  /** Site Code: Site code for which some parameters are estimated. [A4] */
  char *site_code() noexcept { return charbuf__ + site_code_at; }
  const char *site_code() const noexcept { return charbuf__ + site_code_at; }

  /** Point Code: Point Code at a site for which some parameters are estimated.
   * [A2]
   */
  char *point_code() noexcept { return charbuf__ + point_code_at; }
  const char *point_code() const noexcept { return charbuf__ + point_code_at; }

  /** Solution ID: Solution Number at a Site/Point code for which some
   * parameters are estimated. [A4]
   */
  char *soln_id() noexcept { return charbuf__ + soln_id_at; }
  const char *soln_id() const noexcept { return charbuf__ + soln_id_at; }
  int soln_id_int() const noexcept;

  /**  Column tagged 'M' */
  char colm() const noexcept { return charbuf__[colm_at]; }
  char &colm() noexcept { return charbuf__[colm_at]; }

  /**  Column tagged 'A' */
  char cola() const noexcept { return charbuf__[cola_at]; }
  char &cola() noexcept { return charbuf__[cola_at]; }

  /** Comment string */
  char *comment() noexcept { return charbuf__ + comment_at; }
  const char *comment() const noexcept { return charbuf__ + comment_at; }

  /** Time: start of rejection period */
  dso::datetime<dso::nanoseconds> start;

  /** Time: end of rejection period */
  dso::datetime<dso::nanoseconds> stop;

  /** Observation Code: Identification of the observation technique used [A1] */
  SinexObservationCode m_obscode;
}; /* DataReject */

/** @class Hold information stored (per line) in a SITE/ECCENTRICITY Block */
class SiteEccentricity {
private:
  /**
   * Code PT SOLN T Data_start__ Data_end____ AXE Up______ North___ East____
   * ADEA  A    1 D 93:003:00000 98:084:11545 UNE   0.5100   0.0000   0.0000
   */
  static constexpr const int site_code_at = 0;   /* [0,4] including NULL */
  static constexpr const int point_code_at = 5;  /* [5,7] including NULL */
  static constexpr const int soln_id_at = 8;     /* [8,12] including NULL */
  static constexpr const int ref_system_at = 13; /* [13,16] including NULL */
  static constexpr const int ant_serial_at = 34; /* [34,39] including NULL */
  char charbuf__[40] = {'\0'};

  /** [Up or X,  North or Y, East or Z] offset from the marker to the Antenna
   * reference point (ARP). Units are [m].
   */
  double une[3] = {0e0, 0e0, 0e0};

public:
  /** Time: Time since the antenna has been installed at the Site/Point. Value
   * 00:000:00000 indicates that the antenna has been installed at least since
   * the "File Epoch Start Time".
   */
  dso::datetime<dso::nanoseconds> start;

  /** Time: Time until the antenna is installed at a Site/Point. Value
   * 00:000:00000 indicates that the antenna has been installed at least until
   * the "File Epoch End Time".
   */
  dso::datetime<dso::nanoseconds> stop;

  /** Observation Code: Identification of the observation technique used [A1] */
  SinexObservationCode m_obscode;

public:
  /** Site Code: Site code for which some parameters are estimated. [A4] */
  char *site_code() noexcept { return charbuf__ + site_code_at; }
  const char *site_code() const noexcept { return charbuf__ + site_code_at; }

  /** Point Code: Point Code at a site for which some parameters are estimated.
   * [A2]
   */
  char *point_code() noexcept { return charbuf__ + point_code_at; }
  const char *point_code() const noexcept { return charbuf__ + point_code_at; }

  /** Solution ID: Solution Number at a Site/Point code for which some
   * parameters are estimated. [A4]
   */
  char *soln_id() noexcept { return charbuf__ + soln_id_at; }
  const char *soln_id() const noexcept { return charbuf__ + soln_id_at; }
  int soln_id_int() const noexcept;

  /** Eccentricity Reference System: Reference system used to describe vector
   * distance from monument benchmark to the antenna reference point or
   * intersection of axis:
   * 'UNE' - Local reference system [Up, North, East].
   * 'XYZ' - Cartesian Reference System [X, Y, Z].
   * All units are in [m].
   */
  char *ref_system() noexcept { return charbuf__ + ref_system_at; }
  const char *ref_system() const noexcept { return charbuf__ + ref_system_at; }

  /** @brief Return an eccentricity component of choice; index in [0,3) */
  double eccentricity(int index) const noexcept { return une[index]; }
  double &eccentricity(int index) noexcept { return une[index]; }
}; /* SiteEccentricity */

/** @brief Parse a SINEX datetime string.
 *
 * Accepted format is: "YY:DDD:SSSSS", and the Time scale is "UTC",
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
                     const dso::datetime<dso::nanoseconds> &tdefault,
                     dso::datetime<dso::nanoseconds> &t) noexcept;

} /* namespace sinex */
} /* namespace dso */

#endif /* __SINEX_FILE_BLOCK_CLASSES_HPP__ */
