/** @file
 *
 * Define a class to handle equations of post-seismic deformation models. This
 * is closely related to the models used by ITRF, see:
 * https://itrf.ign.fr/ftp/pub/itrf/itrf2020/ITRF2020-PSD-model-eqs-IGN.pdf
 */

#ifndef __DSO_SINEX_ITRF_PSD_MODEL_HPP__
#define __DSO_SINEX_ITRF_PSD_MODEL_HPP__

#include "datetime/calendar.hpp"
#include <cmath>
#include <cstdlib>
#include <cstring>

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
  static constexpr const int MIN_LOGEXP_TERMS = 4;
  static_assert(MIN_LOGEXP_TERMS % 2 == 0);
  static constexpr const int DBLS_IN_TERM = 4;

private:
  /** Number of logarithmic terms. */
  short mnl;
  /** Number of exponential terms. */
  short mne;
  /** Number of logarithmic + exponential terms that can be stored in instance.
   */
  short mcapacity;
  /** Index of term where exponential terms start at m_mem */
  short mexpstart;
  /** data */
  double *mmem;

  static constexpr int pie_start(int mnl_, int mne_, int mcapacity_) noexcept {
    if (mnl_ < MIN_LOGEXP_TERMS / 2)
      return std::min(MIN_LOGEXP_TERMS / 2, mcapacity_ - mne_);
    return mnl_;
  }

  /** @brief Prepare memory for adding a new LOG term in the instance.
   *
   * Note that this method will not actually add a new term, it will also see
   * so that the instance is prepared for the addition of a new LOG term. If
   * needed, more memory will be allocated or things will be moved around so
   * that a new LOG term can be fit in.
   *
   * The number of LOG terms in the instance will remain the same after the
   * call to the function (i.e. mnl will not be augmented).
   *
   * A call to this method should be followed by a call to add_log_term.
   *
   * The function will return a pointer to the first double of the "to be
   * added" term. I.e. If we want to add a new term, we can do something like:
   * double *ptr = prepare_add_log_term();
   * if (!ptr) printf(error ...)
   * *ptr = new_term_amplitude;
   * *(ptr+1) = new_term_tau;
   * *(ptr+2) = new_term_MJD;
   * *(ptr+3) = new_term_sec_of_day;
   * ...
   */
  [[nodiscard]] double *prepare_add_log_term() noexcept {
    if (mnl < mexpstart)
      return mmem + mnl * DBLS_IN_TERM;
    if (mnl + mne < mcapacity) {
      /* we can squeeze one more term, shift mne's to the right. */
      double *src = mmem + (mexpstart - 1) * DBLS_IN_TERM;
      double *dst = src + DBLS_IN_TERM;
      std::size_t sz = mne * DBLS_IN_TERM * sizeof(double);
      std::memmove(dst, src, sz);
      ++mexpstart;
      return mmem + mnl * DBLS_IN_TERM;
    } else {
      auto nptr = (double *)std::realloc(mmem, (mcapacity + 2) * DBLS_IN_TERM *
                                                   sizeof(double));
      if (!nptr) {
        fprintf(stderr, "[ERROR] Failed realloc call (traceback: %s)\n",
                __func__);
        return nullptr;
      }
      mmem = nptr;
      mcapacity += 2;
      return prepare_add_log_term();
    }
  }

  [[nodiscard]] double *prepare_add_exp_term() noexcept {
    if (mexpstart + mne < mcapacity)
      return mmem + (mexpstart + mne) * DBLS_IN_TERM;
    if (mnl < mexpstart) {
      /* we can squeeze one more EXP term; move mexpstart. */
      /* Solution A: this works fine but the newly added EXP term will not
       * be ordered. I.e. the newly added EXP term may be placed before the
       * already existing ones.
      --mexpstart;
      return mmem + mexpstart * DBLS_IN_TERM;
       * Solution B. With this implementation, we are adding the new EXP term
       * in the correct order, i.e. after the already existing EXP terms.
       */
      double *src = mmem + mexpstart * DBLS_IN_TERM;
      double *dst = mmem + (mexpstart - 1) * DBLS_IN_TERM;
      std::size_t sz = mne * DBLS_IN_TERM * sizeof(double);
      std::memmove(dst, src, sz);
      --mexpstart;
      return mmem + (mexpstart + mne) * DBLS_IN_TERM;
    } else {
      /* Nope, we need to allocate more space. */
      auto nptr = (double *)std::realloc(mmem, (mcapacity + 2) * DBLS_IN_TERM *
                                                   sizeof(double));
      if (!nptr) {
        fprintf(stderr, "[ERROR] Failed realloc call (traceback: %s)\n",
                __func__);
        return nullptr;
      }
      mmem = nptr;
      mcapacity += 2;
      return prepare_add_exp_term();
    }
  }

public:
  int num_logarithmic_terms() const noexcept { return mnl; }
  int num_exponential_terms() const noexcept { return mne; }
  /** */
  int start_index_of_exponential_terms_in_data_array() const noexcept {
    return mexpstart;
  }
  int the_instances_capacity() const noexcept { return mcapacity; }
  double *log_term_at(int i) noexcept { return mmem + i * DBLS_IN_TERM; }
  double *exp_term_at(int i) noexcept {
    return mmem + (mexpstart + i) * DBLS_IN_TERM;
  }
  void dummy(int i) {
    for (int j = 0; j <= i; j++) {
      double *ptr = exp_term_at(j);
      printf("[%2d] Amp=%.2f Tau=%.2f MJD=%.2f Sec=%.9f\n", j, *ptr, ptr[1],
             ptr[2], ptr[3]);
    }
  }

  /** @brief Add a new Logarithmic term to the instance. */
  int add_log_term(const MjdEpoch &t, double amp = 0e0,
                   double tau = 0e0) noexcept {
    double *ptr = prepare_add_log_term();
    ptr[0] = amp;
    ptr[1] = tau;
    ptr[2] = static_cast<double>(t.imjd());
    ptr[3] = t.sec_of_day<seconds>();
    return (++mnl);
  }

  /** @brief Add a new Exponential term to the instance. */
  int add_exp_term(const MjdEpoch &t, double amp = 0e0,
                   double tau = 0e0) noexcept {
    double *ptr = prepare_add_exp_term();
    ptr[0] = amp;
    ptr[1] = tau;
    ptr[2] = static_cast<double>(t.imjd());
    ptr[3] = t.sec_of_day<seconds>();
    return (++mne);
  }

  /** @brief Construct instance given the number of logarithmic and/or
   * exponential terms.
   */
  SitePsdModel(int nl = 0, int ne = 0) noexcept
      : mnl(nl), mne(ne), mcapacity(std::max(nl + ne, MIN_LOGEXP_TERMS)),
        mexpstart(pie_start(mnl, mne, mcapacity)),
        mmem((double *)std::malloc(mcapacity * DBLS_IN_TERM * sizeof(double))) {
        };

  /** @brief Copy constructor. */
  SitePsdModel(const SitePsdModel &rh) noexcept
      : mnl(rh.mnl), mne(rh.mne), mcapacity(rh.mcapacity),
        mexpstart(rh.mexpstart),
        mmem((double *)std::malloc(mcapacity * DBLS_IN_TERM * sizeof(double))) {
    std::memcpy(mmem, rh.mmem, mcapacity * DBLS_IN_TERM * sizeof(double));
  };

  /** @brief Move constructor. */
  SitePsdModel(SitePsdModel &&rh) noexcept
      : mnl(rh.mnl), mne(rh.mne), mcapacity(rh.mcapacity),
        mexpstart(rh.mexpstart), mmem(rh.mmem) {
    rh.mne = rh.mnl = rh.mcapacity = rh.mexpstart = 0;
    rh.mmem = nullptr;
  }

  /** @brief Assignment operator. */
  SitePsdModel &operator=(const SitePsdModel &rh) noexcept {
    if (this != &rh) {
      /* do we need to allocate more space? */
      if (mcapacity < (rh.mnl + rh.mne)) {
        // if (mcapacity)
        //   std::free(mmem);
        mcapacity = (rh.mnl + rh.mne) < MIN_LOGEXP_TERMS ? MIN_LOGEXP_TERMS
                                                         : (rh.mnl + rh.mne);
        mmem = (double *)std::realloc(mmem, mcapacity * DBLS_IN_TERM *
                                                sizeof(double));
      }
      mnl = rh.mnl;
      mne = rh.mne;
      mexpstart = pie_start(mnl, mne, mcapacity);
      std::memcpy(mmem, rh.mmem, mnl * DBLS_IN_TERM * sizeof(double));
      std::memcpy(mmem + mexpstart, rh.mmem + rh.mexpstart,
                  mne * DBLS_IN_TERM * sizeof(double));
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
      if (mmem)
        std::free(mmem);
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
