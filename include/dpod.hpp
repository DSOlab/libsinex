/** @file
 * Utilities for dpod file (SINEX, psd and harmonics) provided by the IDS.
 * Mostly used for DORIS beacons/sites.
 *
 * To date, a dpod realization include:
 * 1. A SINEX file which can be handled by the respective dso::Sinex class,
 * 2. A dpod*_freq_corr.txt file, providing anual and semi-anual cartesian
 *    corrections, and
 * 3. A dpod*_psd_corr.txt file, providing PSD corrections in the form of
 *    time-tagged offsets.
 */

#ifndef __DSO_IDS_DPOP_HPP__
#define __DSO_IDS_DPOP_HPP__

#include "sinex.hpp"
#include <cstring>

namespace dso {
/** @brief Parse harmonics off from a dpod_freq_cor file and compute 
 * (cartesian) corrections (ΔX, ΔY, ΔZ).
 *
 * Give a dpod20*_freq_corr.txt file, do the following things:
 * 1. search through all of the sites included in sites_crd and get their 
 *    frequency/harmonics info,
 * 2. compute the total, accumulated harmonics contribution (i.e. for all of 
 *    the included frequencies) in cartesian components (ΔX, ΔY, ΔZ)
 * 3. store results of step 2 in an std::vector<Sinex::SiteCoordinateResults> 
 *    and return it.
 * 
 * Hence, if the function is successeful, to get the 'correct' position of a 
 * given site at the sites_crd vector, the contribution returned from the call 
 * should be added.
 *
 * Note that the resulting vector (std::vector<Sinex::SiteCoordinateResults>) 
 * will have a one-to-one correspondance with the input sites_crd vector, i.e. 
 * the sites will be placed exactly in the same order.
 *
 * Why do we need a std::vector<Sinex::SiteCoordinateResults> as input? Well, 
 * this vector will (probably) hold results/positions from a respective 
 * dpod SINEX file. But, we must know the SOLN_ID, i.e. the solution id that 
 * was used to compute/extrapolate the coordinates. The same solution id must 
 * be used to identify the 'correct' harmonics to be applyied.
 *
 * See for example the following block: 
 * ---------------------------------------------------------------------------
#CODE PT __DOMES__SOLN_XYZ_COSAMP__COSSTD__SINAMP__SINSTD
 REUB  A 97401S002  2   X   2.564   0.183   0.060   0.183
 REUB  A 97401S002  2   Y   2.498   0.151   1.706   0.151
 REUB  A 97401S002  2   Z  -2.041   0.099   0.593   0.099
 ROTA  A 66007S001  2   X   0.287   0.106  -0.930   0.106
 ROTA  A 66007S001  2   Y   1.241   0.086   1.994   0.086
 ROTA  A 66007S001  2   Z  -1.842   0.084  -2.067   0.084
 AJAB  A 10077S002  1   X  -0.000   0.001   0.000   0.001
 AJAB  A 10077S002  1   Y  -0.000   0.001   0.000   0.001
 AJAB  A 10077S002  1   Z   0.000   0.001  -0.000   0.001
 TRIB  A 30604S002  1   X   0.884   0.161  -0.812   0.163
 TRIB  A 30604S002  1   Y   0.409   0.255  -0.705   0.259
 TRIB  A 30604S002  1   Z  -1.148   0.139  -0.276   0.141
 SAKA  A 12329S001  3   X   4.723   0.294   0.807   0.293
 SAKA  A 12329S001  3   Y  -2.538   0.345  -2.942   0.345
 * ---------------------------------------------------------------------------
 * has harmonics insformation w.r.t different solution id's; hence, this file 
 * cannot be treated without prior knowledge of the information in the SINEX 
 * file.
 *
 * @param[in] fn Filename of the dpod20*_freq_corr.txt file
 * @param[in] t Epoch at which to compute the harmonic contributions. If f is 
 *              the frequency of the harmonic, then the harmonic correction is 
 *              computed as: A_{cos} * cos(2*π*fdoy / f)
 *                          +A_{sin} * sin(2*π*fdoy / f)
 * @param[in] sites_crd An std::vector<Sinex::SiteCoordinateResults> where the 
 *            requested sites and solution id's are retrieved from. We are
 *            going to compute harmonic corrections for each of the sites 
 *            included in this vector, tagged with the same solution id as the 
 *            one recorded in here. Wea re matching against (i.e. identifying 
 *            same sites) by: 
 *            - site_code
 *            - point_code
 *            - domes, and
 *            - solution_id
 * @return A std::vector<Sinex::SiteCoordinateResults>, in exactly the same 
 *         order as the input sites_crd vector, but in its (x,y,z) coordinates 
 *         the accumulated frequency corrections are stored (in [m]). In case 
 *         a site is not included in the file or has no harmonic terms, its 
 *         (x,y,z) coordinates will be (0,0,0).
 */
[[nodiscard]]
std::vector<Sinex::SiteCoordinateResults> get_dpod_freq_corr(
    const char *fn, const datetime<dso::nanoseconds> &t,
    const std::vector<Sinex::SiteCoordinateResults> &sites_crd);

/** @brief Parse harmonics off from a dpod_freq_cor file and compute 
 * (cartesian) corrections (ΔX, ΔY, ΔZ).
 *
 * This function does exaclty the same as get_dpod_freq_corr, but instead of 
 * returning the accumulated frequency correcions, it adds them the passed-in 
 * sites_crd vector (inplace).
 *
 * @param[in] fn Filename of the dpod20*_freq_corr.txt file
 * @param[in] t Epoch at which to compute the harmonic contributions. If f is 
 *              the frequency of the harmonic, then the harmonic correction is 
 *              computed as: A_{cos} * cos(2*π*fdoy / f)
 *                          +A_{sin} * sin(2*π*fdoy / f)
 * @param[in] sites_crd An std::vector<Sinex::SiteCoordinateResults> where the 
 *            requested sites and solution id's are retrieved from. We are
 *            going to compute harmonic corrections for each of the sites 
 *            included in this vector, tagged with the same solution id as the 
 *            one recorded in here. We are matching against (i.e. identifying 
 *            same sites) by: 
 *            - site_code
 *            - point_code
 *            - domes, and
 *            - solution_id
 *            At output, the vector will hold the combined input+freq_cor 
 *            terms. I.e., the computed frequency corrections terms will be
 *            added (per site) each of the vector's input holdings.
 * @return An integer, where anything other than 0 denotes an error.
 */
[[nodiscard]]
int apply_dpod_freq_corr(
    const char *fn, const datetime<dso::nanoseconds> &t,
    std::vector<Sinex::SiteCoordinateResults> &sites_crd) noexcept;

[[nodiscard]]
int dpod_extrapolate(const datetime<dso::nanoseconds> &t,
                          const std::vector<const char *> &sites_4charid,
                          const char *dpod_snx,
                          const char *dpod_freq = nullptr) noexcept;
} /* namespace dso */

#endif
