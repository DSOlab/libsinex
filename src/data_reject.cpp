#include "sinex.hpp"
#include <algorithm>
#include <stdexcept>

void dso::sinex::DataReject::add_rejection_interval(
    const dso::sinex::RejectionInterval &ri) {
  if (m_intervals.empty()) {
    m_intervals.emplace_back(ri);
    return;
  }

  // check that intervals do not overlapp, and insert the new
  // RejectionInterval in the sorted vector
  int injection_index = -1;
  for (int i = 0; i < (int)m_intervals.size(); i++) {
    const auto cstart = m_intervals[i].start;
    const auto cend = m_intervals[i].stop;
    if (dso::intervals_overlap<
            dso::seconds,
            dso::datetime_ranges::OverlapComparissonType::AllowEdgesOverlap>(
            cstart, cend, ri.start, ri.stop)) {
      fprintf(stderr,
              "[ERROR] Overlapping interval! Failed to add REJECTION_DATA "
              "interval for site %s (traceback: %s)\n",
              m_site_code, __func__);
      throw std::runtime_error(__func__);
    }
    if (cstart >= ri.stop) {
      injection_index = i;
      break;
    }
  }

  // inject the RejectionInterval in the index injection_index
  if (injection_index > -1) {
    m_intervals.emplace_back(ri);
  } else {
    m_intervals.insert( m_intervals.begin()+injection_index, ri);
  }

  return;
}