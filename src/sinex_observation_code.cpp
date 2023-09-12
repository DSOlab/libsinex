#include "sinex.hpp"
#include <stdexcept>

/* SinexObservationCode to char */
char dso::sinex::SinexObservationCode_to_char(
    dso::sinex::SinexObservationCode soc) {
  switch (soc) {
  case dso::sinex::SinexObservationCode::COMBINED:
    return 'C';
  case dso::sinex::SinexObservationCode::DORIS:
    return 'D';
  case dso::sinex::SinexObservationCode::SLR:
    return 'L';
  case dso::sinex::SinexObservationCode::LLR:
    return 'M';
  case dso::sinex::SinexObservationCode::GNSS:
    return 'P';
  case dso::sinex::SinexObservationCode::VLBI:
    return 'R';
  default:
    throw std::runtime_error("[ERROR] Invalid SINEX Observation Code!\n");
  }
}

/* char to SinexObservationCode (may throw) */
dso::sinex::SinexObservationCode char_to_SinexObservationCode(char c) {
  switch (c) {
  case 'C':
    return dso::sinex::SinexObservationCode::COMBINED;
  case 'D':
    return dso::sinex::SinexObservationCode::DORIS;
  case 'L':
    return dso::sinex::SinexObservationCode::SLR;
  case 'M':
    return dso::sinex::SinexObservationCode::LLR;
  case 'P':
    return dso::sinex::SinexObservationCode::GNSS;
  case 'R':
    return dso::sinex::SinexObservationCode::VLBI;
  default:
    throw std::runtime_error("[ERROR] Invalid SINEX Observation Code!\n");
  }
}

/* char to SinexConstraintCode (may throw) */
dso::sinex::SinexConstraintCode char_to_SinexConstraintCode(char c) {
  switch (c) {
  case '0':
    return dso::sinex::SinexConstraintCode::FIXED;
  case '1':
    return dso::sinex::SinexConstraintCode::SIGNIFICANT;
  case '2':
    return dso::sinex::SinexConstraintCode::UNCONSTRAINED;
  default:
    throw std::runtime_error("[ERROR] Invalid SINEX Constraint Code!\n");
  }
}
