/** @file
 * Utilities: high-level functions for normal, often used operations on SINEX 
 * files.
 */

#ifndef __DSO_SINEX_UTILITY_FUNCTIONS_HPP__
#define __DSO_SINEX_UTILITY_FUNCTIONS_HPP__

#include "sinex.hpp"
#include "dpod.hpp"
#include <vector>

namespace dso {
  int dpod_extrapolate(const std::vector<const char *> &sites_4charid);
} /* namespace dso */

#endif
