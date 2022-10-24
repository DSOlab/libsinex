#include "sinex.hpp"
#include <algorithm>
#include <stdexcept>

void dso::sinex::DataReject::add_rejection_interval(
    const dso::sinex::RejectionInterval &ri) {
  // Well, i tried to make this a sorted, non-overlapping list, but it seems
  // that SINEX files can have overlapping intervals, e.g. dpod2014_057.snx
  // contains the lines:
  //  465  SANB  A    1 D 07:364:00000 08:016:00000 X - Data corrupted
  //  20071230-20080115 466  SANB  A    1 D 07:365:00000 08:016:86399 X - Data
  //  Corrupted 20071231-20080116 467  SANB  A    1 D 10:058:00000 10:087:86399
  //  X - Earthquakes 20100227-20100328
  m_intervals.push_back(ri);
  /*
  if (m_intervals.empty()) {
    m_intervals.push_back(ri);
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
  printf("...inspected and adding at index %d/%d\n", injection_index,
  (int)m_intervals.size());

  // inject the RejectionInterval in the index injection_index
  if (injection_index > -1) {
    m_intervals.insert( m_intervals.begin()+injection_index, ri);
  } else {
    m_intervals.push_back(ri);
  }
  */
  return;
}
