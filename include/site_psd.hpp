/** @file
 *
 * Define a class to handle equations of post-seismic deformation models. This
 * is closely related to the models used by ITRF, see:
 * https://itrf.ign.fr/ftp/pub/itrf/itrf2020/ITRF2020-PSD-model-eqs-IGN.pdf
 */

#ifndef __DSO_SINEX_ITRF_PSD_MODEL_HPP__
#define __DSO_SINEX_ITRF_PSD_MODEL_HPP__

#include <cmath>
#include <cstdlib>
#include <cstring>
#include "datetime/calendar.hpp"

namespace dso {

/**
 *
 * The PSD model is given by:
 * δl(t) = Σ(i=1, nl) Ali * log(1 + (t-tli )/ τli) 
 *       + Σ(i=1, ne) Aei * (1 - exp(-(t - tei)/τei))
 * where:
 *   nl  : Number of logarithmic terms of the parametric model
 *   ne  : Number of exponential terms of the parametric model
 *   Ali : Amplitude of the ith logarithmic term
 *   Aei : Amplitude of the ith exponential term
 *   τli : Relaxation time of the ith logarithmic term
 *   τei : Relaxation time of the ith exponential term
 *   tli : Earthquake time(date) corresponding to ith logarithmic term
 *   tei : Earthquake time(date) corresponding to the ith exponential term
 *
 *   Storage:
 *   * For logarithmic terms, we use 4 doubles as:
 *      1. Amplitude of the ith logarithmic term
 *      2. Relaxation time of the ith logarithmic term
 *      3. Earthquake time(date) corresponding to ith logarithmic term, 
 *         MJD (integer stored as double),
 *      4. Earthquake time(date) corresponding to ith logarithmic term,
 *         (fractional) seconds in day, to go with (3)
 *   * For exponential terms, we use 4 doubles as above.
 */
class SitePsdModel {
private:
  /** Number of logarithmic terms. */
  short mnl; 
  /** Number of exponential terms. */
  short mne;
  /** Number of logarithmic + exponential terms that can be stored in instance. */
  short mcapacity;
  /** Index where exponential terms start at m_mem */
  short mexpstart;
  /** data */
  double *mmem;
public:
  /** @brief Construct instance given the number of logarithmic and/or
   * exponential terms.
   */
  SitePsdModel(int nl = 0, int ne = 0) noexcept
      : mnl(nl), mne(ne), mcapacity(std::max(nl + ne, 4)),
        mexpstart((nl < 2) ? 2 : nl),
        mmem((double *)std::malloc(mcapacity * 4 * sizeof(double))) {};

  /** @brief Copy constructor. */
  SitePsdModel(const SitePsdModel &rh) noexcept
      : mnl(rh.mnl), mne(rh.mne), mcapacity(rh.mcapacity),
        mexpstart(rh.mexpstart),
        mmem((double *)std::malloc(mcapacity * 4 * sizeof(double))) {
    std::memcpy(mmem, rh.mmem, mcapacity * 4 * sizeof(double));
  };

  /** @brief Move constructor. */
  SitePsdModel(SitePsdModel &&rh) noexcept
      : mnl(rh.mnl), mne(rh.mne), mcapacity(rh.mcapacity),
        mexpstart(rh.mexpstart), mmem(rh.mmem) 
  {
    rh.mne = rh.mnl = rh.mcapacity = rh.mexpstart = 0;
    rh.mmem = nullptr;
  }

  /** @brief Assignment operator. */
  SitePsdModel &operator=(const SitePsdModel &rh) noexcept {
    if (this != &rh) {
      /* do we need to allocate more space? */
      if (mcapacity < (rh.mnl + rh.mne)) {
        mcapacity = (rh.mnl + rh.mne) < 4 ? 4 : (rh.mnl + rh.mne);
        mexpstart = (rh.mnl + rh.mne) < 4 ? 2 : rh.mexpstart;
        if (this->mcapacity == 0) {
          mmem = (double *)std::malloc(mcapacity * 4 * sizeof(double));
        } else {
          mmem = (double *)std::realloc(mmem, mcapacity * 4 * sizeof(double));
        }
      } else {
        mexpstart = (rh.mnl + rh.mne) < 4 ? 2 : rh.mexpstart;
      }
      mnl = rh.mnl;
      mne = rh.mne;
      std::memcpy(mmem, rh.mmem, mnl * 4 * sizeof(double));
      std::memcpy(mmem + mexpstart, rh.mmem + rh.mexpstart,
                  mne * 4 * sizeof(double));
    }
    return *this;
  }

  /** @brief Move assignment operator. */
  SitePsdModel &operator=(SitePsdModel &&rh) noexcept {
    if (this != &rh) {
      mnl = rh.mnl;
      mne = rh.mne;
      mcapacity = rh.mcapacity;
      mexpstart = rh.mexpstart;
      if (mmem) std::free(mmem);
      mmem = rh.mmem;
      rh.mne = rh.mnl = rh.mcapacity = rh.mexpstart = 0;
      rh.mmem = nullptr;
    }
    return *this;
  }

  /** @brief Destructor. */
  ~SitePsdModel() noexcept {
    if (mcapacity)
      std::free(mmem);
  }
}; /* class SitePsdModel */

} /* namespace dso */

#endif
