#include "sinex.hpp"
#include <charconv>
#include <cstdio>

namespace {
const char *ltrim__(const char *str) noexcept {
  while (*str && (*str == ' '))
    ++str;
  return str;
}

int soln_id_int_generic(const char *str) noexcept {
  int solnid = 0;
  const auto [ptr, ec] =
      std::from_chars(ltrim__(str),
                      str + dso::sinex::SOLN_ID_CHAR_SIZE, solnid);
  if (ec != std::errc{}) {
    solnid = dso::sinex::NONINT_SOLN_ID;
    fprintf(stderr,
            "[WRNNG] Non-integer solution identifier! (traceback: %s)\n",
            __func__);
  }
  return solnid;
}
} /* unnamed namespace */

int dso::sinex::SiteReceiver::soln_id_int() const noexcept {
  return soln_id_int_generic(this->soln_id());
}
int dso::sinex::SiteAntenna::soln_id_int() const noexcept {
  return soln_id_int_generic(this->soln_id());
}
int dso::sinex::SolutionEstimate::soln_id_int() const noexcept {
  return soln_id_int_generic(this->soln_id());
}
int dso::sinex::SolutionEpoch::soln_id_int() const noexcept {
  return soln_id_int_generic(this->soln_id());
}
int dso::sinex::DataReject::soln_id_int() const noexcept {
  return soln_id_int_generic(this->soln_id());
}
int dso::sinex::SiteEccentricity::soln_id_int() const noexcept {
  return soln_id_int_generic(this->soln_id());
}
int dso::Sinex::SiteCoordinateResults::soln_id_int() const noexcept {
  return soln_id_int_generic(this->soln_id());
}
