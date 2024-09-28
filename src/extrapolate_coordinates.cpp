#include "sinex.hpp"
#include "sinex_blocks.hpp"
#include <datetime/calendar.hpp>
#include <limits>

constexpr const double EMPTY = std::numeric_limits<double>::min();

int dso::Sinex::linear_extrapolate_coordinates(
    const std::vector<sinex::SiteId> &sites,
    const dso::datetime<dso::nanoseconds> &t,
    std::vector<dso::Sinex::SiteCoordinateResults> &crd) noexcept {
  if (!crd.empty())
    crd.clear();
  crd.reserve(sites.size());

  /* get (all recorded) estimates for each of the sites given */
  std::vector<dso::sinex::SolutionEstimate> sols;
  if (this->parse_block_solution_estimate(sites, t,
                                          /*allow_extrapolation*/ true, sols)) {
    fprintf(stderr,
            "[ERROR] Failed parsing site solution from SINEX file %s "
            "(traceback: %s)\n",
            m_filename.c_str(), __func__);
    return 1;
  }

  const char *p[] = {"STAX", "VELX", "STAY", "VELY", "STAZ", "VELZ"};
  double xyz[3];

  /* loop through all sites (i.e. SiteId's) */
  for (const auto &site : sites) {
    /* initialize coordinates to a dummy value */
    xyz[0] = xyz[1] = xyz[2] = EMPTY;
    /* loop through site components */
    for (int xcomponent = 0; xcomponent < 6; xcomponent += 2) {
      const char *sta = p[xcomponent];
      const char *vel = p[xcomponent + 1];
      /* find estimates for component */
      auto xit = std::find_if(sols.begin(), sols.end(),
                              [&](const dso::sinex::SolutionEstimate &s) {
                                return s.match_site(site) &&
                                       !(std::strcmp(sta, s.parameter_type()));
                              });
      /* find estimates for velocity */
      auto vit = std::find_if(sols.begin(), sols.end(),
                              [&](const dso::sinex::SolutionEstimate &s) {
                                return s.match_site(site) &&
                                       !(std::strcmp(vel, s.parameter_type()));
                              });
      /* check for wierd stuff ...*/
      if (xit != sols.end() && vit == sols.end()) {
        fprintf(stderr,
                "[ERROR] Found only %s but not %s parameter for site %s %s; "
                "SINEX: %s "
                "(traceback: %s)\n",
                sta, vel, site.site_code(), site.point_code(),
                m_filename.c_str(), __func__);
        return 1;
      } else if (xit == sols.end() && vit != sols.end()) {
        fprintf(stderr,
                "[ERROR] Found only %s but not %s parameter for site %s %s; "
                "SINEX: %s "
                "(traceback: %s)\n",
                vel, sta, site.site_code(), site.point_code(),
                m_filename.c_str(), __func__);
        return 1;
      } else {
        /* ok, we have coordinates and velocity for component */
        const double x0 = xit->estimate();
        const double vx = vit->estimate();
        const auto dt =
            t.diff<dso::DateTimeDifferenceType::FractionalYears>(xit->epoch());
        auto val = x0 + vx * dt.years();
        xyz[xcomponent / 2] = val;
      }
    } /* end looping components for the site */

    /* did we collect all components ? */
    if ((xyz[0] == EMPTY) || (xyz[1] == EMPTY) || (xyz[2] == EMPTY)) {
      fprintf(stderr,
              "[ERROR] Failed retrieving components for site %s %s; "
              "SINEX: %s "
              "(traceback: %s)\n",
              site.site_code(), site.point_code(), m_filename.c_str(),
              __func__);
      return 1;
    }

    /* append to result vector */
    crd.emplace_back(
        dso::Sinex::SiteCoordinateResults(site, xyz[0], xyz[1], xyz[2]));
  } /* end looping sites */

  return 0;
}
