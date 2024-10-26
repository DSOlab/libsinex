/** @file
 *
 * Define a class to hold Harmonic coefficients, i.e. signals/models of type:
 * y = Ai * sin (2πf*t) + Aj * cos (2πf*t)
 */

#ifndef __DSO_SINEX_HARMONIC_MODEL_HPP__
#define __DSO_SINEX_HARMONIC_MODEL_HPP__

#include <cmath>
#include <cstdlib>
#include <cstring>

namespace dso {

class RealHarmonics {
  static constexpr const int MIN_HARMONIC_TERMS = 2;
  static constexpr const int DBLS_IN_TERM = 3;

private:
  /** @brief Number of harmonics/frequencies currently stored. */
  int m_num_harmonics{0};
  /** @brief Current capacity. */
  int m_capacity;
  /** @brief Pointer to data, 3 doubles for each frequency. Order is:
   * 1st double: frequency,
   * 2nd double: sin amplitude,
   * 3rd double: cos amplitude
   */
  double *m_mem;

  /** @brief Resize instance to hold num_harmonics harmonic constituents. */
  int resize(int num_harmonics) noexcept {
    if (num_harmonics <= m_capacity) {
      return (m_num_harmonics = num_harmonics);
    } else {
      m_mem = (double *)std::realloc(
          (void *)m_mem, num_harmonics * DBLS_IN_TERM * sizeof(double));
      return (m_num_harmonics = m_capacity = num_harmonics);
    }
  }

public:
  /** @brief Add new constituent, given the frequency and amplitudes.
   * @return The (new) number of harmonics in the instance.
   */
  int add_harmonic(double freq, double amp_sin = 0e0,
                   double amp_cos = 0e0) noexcept {
    if (m_capacity < m_num_harmonics + 1)
      this->resize(m_num_harmonics + 1);
    else
      ++m_num_harmonics;
    /* note that m_num_harmonics has changed to a new value (from resize). */
    m_mem[(m_num_harmonics - 1) * DBLS_IN_TERM + 0] = freq;
    m_mem[(m_num_harmonics - 1) * DBLS_IN_TERM + 1] = amp_sin;
    m_mem[(m_num_harmonics - 1) * DBLS_IN_TERM + 2] = amp_cos;
    return m_num_harmonics;
  }

  /** @brief Accumulate value of model at given t.
   *
   * @return Σ(i=1,num_harmonics){Asi * sin(2πfi*t) + Aci * cos(2πfi*t)}
   */
  double value(double t) const noexcept {
    double sum = 0e0;
    for (int j = 0; j < m_num_harmonics; j++) {
      const double f = m_mem[j * DBLS_IN_TERM + 0];
      const double As = m_mem[j * DBLS_IN_TERM + 1];
      const double Ac = m_mem[j * DBLS_IN_TERM + 2];
      const double s = std::sin(2e0 * M_PI * f * t);
      const double c = std::cos(2e0 * M_PI * f * t);
      sum += As * s + Ac * c;
    }
    return sum;
  }

  int num_harmonics() const noexcept { return m_num_harmonics; }
  const double *operator()(int i) const noexcept {
    return m_mem + i * DBLS_IN_TERM;
  }
  double *operator()(int i) noexcept { return m_mem + i * DBLS_IN_TERM; }

  /** @brief Construct instance given the number of harmonics. */
  RealHarmonics(int num_harmonics = 0) noexcept
      : m_num_harmonics(num_harmonics),
        m_capacity(num_harmonics > MIN_HARMONIC_TERMS ? num_harmonics
                                                      : MIN_HARMONIC_TERMS),
        m_mem((double *)std::malloc(m_capacity * DBLS_IN_TERM *
                                    sizeof(double))) {};

  /** @brief Copy constructor. */
  RealHarmonics(const RealHarmonics &rh) noexcept
      : m_num_harmonics(rh.m_num_harmonics),
        m_capacity(rh.m_num_harmonics > MIN_HARMONIC_TERMS
                       ? rh.m_num_harmonics
                       : MIN_HARMONIC_TERMS),
        m_mem(
            (double *)std::malloc(m_capacity * DBLS_IN_TERM * sizeof(double))) {
    std::memcpy(m_mem, rh.m_mem,
                rh.m_num_harmonics * DBLS_IN_TERM * sizeof(double));
  };

  /** @brief Move constructor. */
  RealHarmonics(RealHarmonics &&rh) noexcept
      : m_num_harmonics(rh.m_num_harmonics), m_capacity(rh.m_num_harmonics),
        m_mem(rh.m_mem) {
    rh.m_num_harmonics = 0;
    rh.m_capacity = 0;
    rh.m_mem = nullptr;
  }

  /** @brief Assignment operator. */
  RealHarmonics &operator=(const RealHarmonics &rh) noexcept {
    if (this != &rh) {
      /* do we need to allocate more space? */
      if (this->m_capacity < rh.m_capacity) {
        if (this->m_capacity == 0) {
          m_mem = (double *)std::malloc(rh.m_capacity * DBLS_IN_TERM *
                                        sizeof(double));
        } else {
          m_mem = (double *)std::realloc(m_mem, rh.m_capacity * DBLS_IN_TERM *
                                                    sizeof(double));
        }
        m_capacity = rh.m_capacity;
      }
      m_num_harmonics = rh.m_num_harmonics;
      std::memcpy(m_mem, rh.m_mem,
                  rh.m_capacity * DBLS_IN_TERM * sizeof(double));
    }
    return *this;
  }

  /** @brief Move assignment operator. */
  RealHarmonics &operator=(RealHarmonics &&rh) noexcept {
    if (this != &rh) {
      m_num_harmonics = rh.m_num_harmonics;
      m_capacity = rh.m_capacity;
      if (m_capacity)
        std::free(m_mem);
      m_mem = rh.m_mem;
      rh.m_num_harmonics = 0;
      rh.m_capacity = 0;
      rh.m_mem = nullptr;
    }
    return *this;
  }

  /** @brief Destructor. */
  ~RealHarmonics() noexcept {
    if (m_capacity)
      std::free(m_mem);
  }

}; /* class RealHarmonics */

} /* namespace dso */

#endif
