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

#include "real_harmonics.hpp"
#include "sinex.hpp"
#include <cstring>

namespace dso {
int apply_dpod_freq_corr(
    const char *fn, 
    const dso::datetime<dso::nanoseconds> &t,
    const std::vector<Sinex::SiteCoordinateResults> &sites_crd) noexcept;
} /* namespace dso */

#endif
