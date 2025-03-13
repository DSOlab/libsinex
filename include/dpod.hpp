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
#include "real_harmonics.hpp"
#include <cstring>

namespace dso {
  int parse_dpod_freq_corr(const char *fn,
      const std::vector<sinex::SiteId> &sites_vec,
      std::vector<SiteRealHarmonics> &harm) noexcept;
} /* namespace dso */

#endif
