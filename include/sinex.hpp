/** @file
 * Public interface for interacting with SINEX files.
 */

#ifndef __SINEX_FILE_PARSER_HPP__
#define __SINEX_FILE_PARSER_HPP__

#include "sinex_blocks.hpp"
#include <type_traits>
#include <vector>
#ifdef DEBUG
#include "datetime/datetime_write.hpp"
#include <cstdio>
#endif

namespace dso {

/** An (input) SINEX class
 *
 * This class acts as an interface for reading/parsing SINEX files and
 * extracting all relevant information
 */
class Sinex {
private:
  using pos_t = std::ifstream::pos_type;

  /** SINEX filename */
  std::string m_filename;
  /** input stream (opened at c'tor) */
  std::ifstream m_stream;
  /** format version */
  float m_version;
  /** agency creating the file [A3] */
  char m_agency[4] = {'\0'};
  /** Identify the agency providing the data in the SINEX file [A3] */
  char m_data_agency[4] = {'\0'};
  /** Solution Contents */
  char m_sol_contents[6] = {'\0'};
  /** Creation time of this SINEX file */
  dso::datetime<dso::nanoseconds> m_created_at;
  /** Start time of the data used in the SINEX solution */
  dso::datetime<dso::nanoseconds> m_data_start;
  /** End time of the data used in the SINEX solution */
  dso::datetime<dso::nanoseconds> m_data_stop;
  /** Technique(s) used to generate the SINEX solution */
  sinex::SinexObservationCode m_obscode;
  /** Single character indicating the constraint in the SINEX solution. */
  sinex::SinexConstraintCode m_constraint_code;
  /** Number of parameters estimated in this SINEX file */
  long m_num_estimates;
  /** Markers for easily accesing blocks. The entries here mark SINEX
   * block-positions and block-types. When placing the stream at a the
   * m_blocks[n], that means that:
   * We place the (input) stream poisition at m_blocks[n].mpos, i.e. the end
   * of the previous line, prior to the start of the new block (this means
   * that the next line to be read is the starting of a new block, e.g.
   * '+SOLUTION/EPOCHS'). Next line to be read is start of block described by
   * m_blocks[n].mtype.
   */
  std::vector<sinex::SinexBlockPosition> m_blocks;

  /** @brief Parse first SINEX line (header) and assign instance's member vars
   */
  int parse_first_line() noexcept;

  /** @brief Read the SINEX file through, and mark all positions of interest
   *       (i.e. start of blocks).
   * This function will fill in the m_blocks vector and perform a basic
   * sanity check of the SINEX file. Note that m_blocks does not contain
   * positions of the 'start of a block line', but rather positions of
   * 'eol before a new block line'.
   * This function should only be called once, at the instance's ctor.
   */
  int mark_blocks() noexcept;

  /** @brief Go (i.e. place the stream) at the the start of a block in a SINEX
   *        instance.
   * Asserts that the mark_blocks() function has already been called. Example:
   * if (goto_block("SOLUTION/EPOCHS")) return 1;
   * Now, next line to be read is: "+SOLUTION/EPOCHS"
   *
   * @param[in] A valid SINEX block (see e.g. dso::sinex::block_names[]);
   *            expects a NULL terminated C-string.
   */
  int goto_block(const char *block) noexcept;

  /** @brief Given a block name, find the relevant entry in the m_blocks
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

  /** @brief Get SOLUTION/EPOCHS for given sites and epoch
   *
   * Parse the SINEX block SOLUTION/EPOCHS and return a vector of
   * sinex::SolutionEpoch instances for the sites included in site_vec, valid
   * at the epoch t.
   *
   * The sites of interest are the ones included in the sites_vec (input)
   * vector. Any SOLUTION/EPOCHS line for which we have a matching SITE_ID and
   * POINT_ID will be inspected. If the input time t matches (i.e. lies within) 
   * the SOLUTION/EPOCHS recorded interval, then the record will be collected 
   * and returned in the out_vec.
   *
   * @param[in] site_vec A list of SITE/ID instances that shall be considered.
   *              We will be matching records according to SITE_CODE and
   *              POINT_CODE.
   * @param[in] t Epoch for which we want the solution record (SOLUTION/EPOCHS
   *              line). For a solution record to be collected, the relation:
   *              SOLUTION_ID_START <= t < SOLUTION_ID_STOP
   *              must hold.
   * @param[out] out_vec A vector of sinex::SolutionEpoch instances for some
   *              or all of the sites contained in site_vec, valid for the
   *              time given (i.e. t)
   * @return Anything other than zero denotes an error
   */
  int parse_solution_epoch_noextrapolate(
      const std::vector<sinex::SiteId> &site_vec,
      const dso::datetime<dso::nanoseconds> &t,
      std::vector<dso::sinex::SolutionEpoch> &out_vec) noexcept;

  /** @brief Get SOLUTION/EPOCHS records
   *
   * Parse the SINEX block SOLUTION/EPOCHS and return a vector of
   * sinex::SolutionEpoch instances for the sites included in site_vec.
   *
   * The sites of interest are the ones included in the sites_vec (input)
   * vector. Any SOLUTION/EPOCHS line for which we have a matching SITE_ID and
   * POINT_ID will be inspected. 
   *
   * The solution to be collected for each site, will be the one with a 
   * start/stop interval closest to t (i.e. t does not have to lay within the 
   * interval:
   * SOLUTION_ID_START <= t < SOLUTION_ID_STOP.
   * This means that if a site has only one SOLUTION/EPOCH record, then this
   * will be collected. If it has multiple records, then the one with the
   * closest interval to t will be collected.
   *
   * @param[in] site_vec A list of SITE/ID instances that shall be considered.
   *              We will be matching records according to SITE_CODE and
   *              POINT_CODE.
   * @param[in] t Epoch for which we want the solution record (SOLUTION/EPOCHS
   *              line).
   * @param[out] out_vec A vector of sinex::SolutionEpoch instances for some
   *              or all of the sites contained in site_vec; the 
   *              sinex::SolutionEpoch instances may not satisfy 
   *              SOLUTION_ID_START <= t < SOLUTION_ID_STOP, but will be the 
   *              closest ones to the time given (i.e. t).
   * @return Anything other than zero denotes an error
   */
  int parse_solution_epoch_extrapolate(
      const std::vector<sinex::SiteId> &site_vec,
      const dso::datetime<dso::nanoseconds> &t,
      std::vector<dso::sinex::SolutionEpoch> &out_vec) noexcept;

public:
  /** return the SINEX filename */
  std::string filename() const noexcept { return m_filename; }

  /** @brief Get SITE/ID records for given sites.
   *
   * Parse the SITE/ID block of the SINEX file and collect info for given
   * sites.
   * This function will search through the SITE/ID block, and collect all
   * info for the sites that are included in the sites vector. Matching of
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

  /** @brief Parse the (whole) SITE/ID block of the SINEX and return all info
   * @param[out] site_vec A vector containing one SiteId entry for each of the
   *            the sites that are included in the block.
   * @return Anything other than 0 denotes an error
   */
  int parse_block_site_id(std::vector<sinex::SiteId> &site_vec) noexcept {
    return parse_block_site_id(std::vector<const char *>(), false, site_vec);
  }

  /** @brief Parse the whole SITE/RECEIVER Block off from the SINEX instance.
   * @param[out] site_vec A vector of sinex::SiteReceiver instances, one
   *               entry for each block line.
   * @return Anything other than zero denotes an error
   */
  int parse_block_site_receiver(
      std::vector<sinex::SiteReceiver> &site_vec) noexcept;

  /** @brief Parse the whole SITE/ANTENNA Block off from the SINEX instance.
   *
   * @param[out] site_vec A vector of sinex::SiteAntenna instances, one
   *               entry for each block line.
   * @return Anything other than zero denotes an error
   */
  int parse_block_site_antenna(
      const std::vector<sinex::SiteId> &sites_vec,
      std::vector<sinex::SiteAntenna> &out_vec,
      const dso::datetime<dso::nanoseconds> from =
          dso::datetime<dso::nanoseconds>::min(),
      const dso::datetime<dso::nanoseconds> to =
          dso::datetime<dso::nanoseconds>::max()) noexcept;

  /** @brief Get SOLUTION/ESTIMATE records for given sites.
   *
   * Parse the whole SOLUTION/ESTIMATE block off from the SINEX
   * instance and collect sinex::SolutionEstimate records for the SITES of
   * interest. The sites of interest are the ones included in the sites_vec
   * (input) vector. Any SOLUTION/ESTIMATE line for which we have a matching
   * SITE ID and POINT ID will be collected, regardless of parameter type and 
   * validity interval.
   *
   * @param[in] sites_vec A vector of sinex::SiteId instances to match against,
   *             using the SITE CODE and POINT CODE fields.
   * @param[out] estimates_vec A vector of sinex::SolutionEstimate instances, 
   *             one entry for each block line. Of course, for one site, aka 
   *             one sites_vec entry, there will be a few entries in 
   *             estimates_vec, depending on available parameters and validity
   *             intervals.
   * @return Anything other than zero denotes an error
   */
  int parse_block_solution_estimate(
      const std::vector<sinex::SiteId> &sites_vec,
      std::vector<sinex::SolutionEstimate> &estimates_vec) noexcept;

  /** Get SOLUTION/ESTIMATE records for given sites and epoch.
   *
   * Parse the SOLUTION/ESTIMATE block off from the SINEX instance and collect
   * sinex::SolutionEstimate records for the sites of interest valid (in some
   * way) at a given epoch. Any parameter type recorded in block, will be 
   * collected, no filtering on that.
   *
   * @param[in] sites A vector of sinex::SiteId instances to match against,
   *            using the SITE_CODE and POINT_CODE fields.
   * @param[in] t The epoch to extract solutions/estimates for. Depending on
   *              the parameter @p allow_extrapolation the epoch is used to
   *              check if a record is temporaly valid.
   * @param[in] allow_extrpolation This parameter signals what we mean by the
   *            phrase "a solution/estimate is valid at t". I.e.
   *               * if set to false, then for a SOLUTION/ESTIMATE record to
   *                 be valid at the given t, the record's data start and data
   *                 stop entries should include t, i.e. the condition
   *                 data_start <= t <= data_stop should be met. In any other
   *                 occasion, the record will not be collected (not considered
   *                 a match).
   *               * if set to true, the then condition
   *                 data_start <= t <= data_stop is not a prerequisite for a
   *                 SOLUTION/ESTIMATE record to be considered a match. The
   *                 function will actually parse all SOLUTION/ESTIMATE records
   *                 for a given site, and collect the one closest to t. That
   *                 is, it will assume that the SOLUTION/ESTIMATE record is
   *                 valid forwaard/backward in time.
   *            Note that this function will call parse_solution_epoch and this 
   *            parameter (i.e. @p allow_extrpolation will be forwarded to this 
   *            call).
   * @param[out] estimates The collected SOLUTION/ESTIMATE records for the
   *             given site list, valid (in some way) at epoch t.
   * @return Anything other than zero denotes an error
   */
  int parse_block_solution_estimate(
      const std::vector<sinex::SiteId> &sites,
      const dso::datetime<dso::nanoseconds> &t, bool allow_extrapolation,
      std::vector<sinex::SolutionEstimate> &estimates) noexcept;

  /** @brief Parse the SOLUTION/DATA_REJECT Block for given sites and date.
   *
   * Parse the whole SOLUTION/DATA_REJECT Block off from the SINEX instance
   * and collect sinex::DataReject instances for the SITES of interest. The
   * sites of interest are the ones included in the sites_vec (input) vector.
   * Any SOLUTION/DATA_REJECT line for which we have a matching SITE ID and
   * POINT ID will be collected, so long as it falls within or overlaps the
   * given interval [from, to].
   *
   * @note The sinex::DataReject instance's start and end times (for data
   *       rejection) shall not differ (at output) from the ones recorded in
   *       the SINEX file, even if the line only includes a sub-interval of
   *       [from, to]. For exmple:
   * auto t1 = datetime<seconds>(year(2005), day_of_year(349), seconds(0));
   * auto t2 = datetime<seconds>(year(2005), day_of_year(351), seconds(0));
   * snx.parse_block_data_reject(sites, datarej, t1, t2));
   *       At output, datarej will hold the interval:
   * DIOA 2005-12-15 00:00:00 2006-05-16 23:59:59 Transmission stopped
   *       where the relevant SINEX line is:
   * DIOA  A    1 D 05:349:00000 06:136:86399 X - Transmission stopped
   *       Here, [from,to] = [2005/349, 2005/351] is only a sub-interval of
   *       the rejection period [2005/349, 2006/136] recorded in the SINEX
   *       file. However, the whole period is stored in the instance.
   *
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
  int parse_block_data_reject(
      const std::vector<sinex::SiteId> &site_vec,
      std::vector<sinex::DataReject> &out_vec,
      const dso::datetime<dso::nanoseconds> from =
          dso::datetime<dso::nanoseconds>::min(),
      const dso::datetime<dso::nanoseconds> to =
          dso::datetime<dso::nanoseconds>::max()) noexcept;

  /** @brief Read and parse the SITE/ECCENTRICITY block off from the SINEX
   * instance.
   *
   * @param[in] site_vec A list of SITE/ID instances that shall be considered.
   *            We will be matching records according to SITE CODE and
   *            POINT CODE.
   * @param[in] t  The time at which we want the eccentricities. If later
   *            than the instance's DATA STOP time, then we will act according
   *            to the allow_extrapolation variable. If set to false, then no
   *            extrapolation is performed and if t is later than the record's
   *            Data End field, the record will not be collected.
   *            If set to true, we will be assuming that the validity
   *            intervals that end later than (DATA_STOP-allowed_offset) are
   *            valid internaly in the future.
   * @param[out] out_vec A vector of sinex::SiteEccentricity for some or all
   *            of the sites contained in site_vec, valid for the time given
   *            (ie t)
   * @param[in] allow_extrapolation Allow extrapolation of eccentricity after
   *            DATA_STOP, in case a record is valid up to
   *            (DATA_STOP-allowed_offset). See \t t.
   * @param[in] allowed_offset See \t t and \t allow_extrapolation
   * @return Anything other than 0 denotes an error
   */
  int parse_block_site_eccentricity(
      const std::vector<sinex::SiteId> &site_vec,
      const dso::datetime<dso::nanoseconds> &t,
      std::vector<sinex::SiteEccentricity> &out_vec,
      bool allow_extrapolation = true,
      FractionalSeconds allowed_offset = FractionalSeconds(2e0)) noexcept;

  /** @brief SOLUTION/EPOCHS for given sites and epoch.
   *
   * Parse the SINEX block SOLUTION/EPOCHS and return a vector of
   * sinex::SolutionEpoch instances for the sites included in site_vec.
   *
   * @param[in] site_vec A list of SITE/ID instances that shall be considered.
   *              We will be matching records according to SITE CODE and
   *              POINT CODE.
   * @param[in] t Epoch for which we want the solution record (SOLUTION/EPOCHS
   *              line).
   * @param[in] allow_extrapolation If set to false, then for a solution
   *              record to be valid, the relation
   *              SOLUTION ID START <= t < SOLUTION ID STOP
   *              must hold. If set to true, then the solution record with the
   *              interval closest to t will be collected (even if t does not
   *              lay within its specified observation interval).
   * @param[out] out_vec A vector of sinex::SolutionEpoch instances for some
   *              or all of the sites contained in site_vec
   * @return Anything other than zero denotes an error
   */
  int parse_solution_epoch(
      const std::vector<sinex::SiteId> &site_vec,
      const dso::datetime<dso::nanoseconds> &t, bool allow_extrapolation,
      std::vector<dso::sinex::SolutionEpoch> &out_vec) noexcept {
    return (allow_extrapolation)
               ? this->parse_solution_epoch_extrapolate(site_vec, t, out_vec)
               : this->parse_solution_epoch_noextrapolate(site_vec, t, out_vec);
  }

  /* TODO obsolete */
  int get_solution_estimate(const char *site_codes[],
                            const dso::datetime<dso::nanoseconds> &t,
                            bool error_if_missing = false) noexcept;

  struct SiteCoordinateResults {
    sinex::SiteId msite;
    /* null-terminated string of solution id, with size 4+1 characters. this
     * should correspond to the solution/estimate solution id that this 
     * instance was constructed from.
     */
    char msolnid[5] = {'\0'};
    /* coordinates in [m] in [X,Y,Z] components */
    double x, y, z; 
    SiteCoordinateResults(const sinex::SiteId &s, const char *solnid, double mx, double my,
                          double mz) noexcept
        : msite(s), x(mx), y(my), z(mz) {
          std::strcpy(msolnid,solnid);
          }
    const char *soln_id() const noexcept {return msolnid;}
    int soln_id_int() const noexcept;
  }; /* SiteCoordinateResults */

  /** @brief Extrpolate coordinate estimates for a given epoch.
   *
   * For the list of sites given, find their solutions/estimates valid for the
   * given epoch, and extrapolate the solution to epoch (t). SITE IDs and
   * POINT IDs will be used to match given sites. If there are multiple
   * solutions for a given site (with different data periods), the function
   * will collect the solution with data span closest to (or valid at) the
   * given epoch, but note that it is allowed to extrapolate both forward and
   * backwards in time.
   * The parameters to be collected are "STAX", "VELX", "STAY", "VELY", "STAZ"
   * and "VELZ", and all of them should be present in the SINEX file. A strict
   * linear model is assumed here (e.g. PDS parameters will not be considered
   * even if present).
   *
   * @param[in] sites A vector of sinex::SiteId instances to match against,
   *               using the SITE CODE and POINT CODE fields.
   * @param[in] t The epoch to extract solutions/estimates for. Note that if
   *               multiple solutions/estimates for a given site are present,
   *               then we will be chosing the one closest to @p t.
   * @param[out] crd A vector holding extrapolation results
   * @return Anything other than zero denotes an error.
   */
  int linear_extrapolate_coordinates(
      const std::vector<sinex::SiteId> &sites,
      const dso::datetime<dso::nanoseconds> &t,
      std::vector<SiteCoordinateResults> &crd) noexcept;

  /** @brief Constructor (may throw). This will:
   * 1. Assign filename,
   * 2. open the stream,
   * 3. parse_first_line() to assign member vars,
   * 4. call mark_blocks() to fill in m_blocks
   */
  Sinex(const char *fn);

  /** @brief Copy not allowed */
  Sinex(const Sinex &) = delete;

  /** @brief Assignment not allowed */
  Sinex &operator=(const Sinex &) = delete;

  /** @brief Destructor */
  ~Sinex() noexcept {
    if (m_stream.is_open())
      m_stream.close();
  }

}; /* Sinex */

int filter_solution_estimates(
    const std::vector<dso::sinex::SolutionEstimate> &estimates,
    const std::vector<dso::sinex::SolutionEpoch> &epochs,
    std::vector<dso::sinex::SolutionEstimate> &filtered_estimates) noexcept;

} /* namespace dso */

#endif /*__SINEX_FILE_PARSER_HPP__*/
