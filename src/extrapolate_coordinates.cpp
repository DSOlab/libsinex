#include "datetime/calendar.hpp"
#include "sinex.hpp"
#include "sinex_blocks.hpp"
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
  for (auto site = sites.cbegin(); site != sites.end(); ++site) {
    /* initialize coordinates to a dummy value */
    xyz[0] = xyz[1] = xyz[2] = EMPTY;
    char solnid[sinex::SOLN_ID_CHAR_SIZE + 1] = {'\0'};
    /* loop through site components */
    for (int xcomponent = 0; xcomponent < 6; xcomponent += 2) {
      const char *sta = p[xcomponent];     // e.g. "STAX"
      const char *vel = p[xcomponent + 1]; // e.g. "VELX"
      /* find estimates for position component */
      auto xit = std::find_if(
          sols.begin(), sols.end(), [&](const dso::sinex::SolutionEstimate &s) {
            return ((!std::strncmp(site->point_code(), s.point_code(),
                                   sinex::POINT_CODE_CHAR_SIZE) &&
                     !std::strncmp(site->site_code(), s.site_code(),
                                   sinex::SITE_CODE_CHAR_SIZE)) &&
                    !std::strncmp(sta, s.parameter_type(),
                                  sinex::PARAMETER_TYPE_CHAR_SIZE));
          });
      /* find estimates for velocity component */
      auto vit = std::find_if(
          sols.begin(), sols.end(), [&](const dso::sinex::SolutionEstimate &s) {
            return ((!std::strncmp(site->point_code(), s.point_code(),
                                   sinex::POINT_CODE_CHAR_SIZE) &&
                     !std::strncmp(site->site_code(), s.site_code(),
                                   sinex::SITE_CODE_CHAR_SIZE)) &&
                    !std::strncmp(vel, s.parameter_type(),
                                  sinex::PARAMETER_TYPE_CHAR_SIZE));
          });
      /* we should have both terms of the linear model */
      if (xit == sols.end() || vit == sols.end()) {
        if (xit == sols.end() && vit == sols.end()) {
          fprintf(
              stderr,
              "[ERROR] Failed to find both %s and %s parameter for site %s %s; "
              "SINEX: %s "
              "(traceback: %s)\n",
              sta, vel, site->site_code(), site->point_code(),
              m_filename.c_str(), __func__);
          return 1;
        } else if (xit != sols.end() && vit == sols.end()) {
          fprintf(stderr,
                  "[ERROR] Found only %s but not %s parameter for site %s %s; "
                  "SINEX: %s "
                  "(traceback: %s)\n",
                  sta, vel, site->site_code(), site->point_code(),
                  m_filename.c_str(), __func__);
          return 1;
        } else if (xit == sols.end() && vit != sols.end()) {
          fprintf(stderr,
                  "[ERROR] Found only %s but not %s parameter for site %s %s; "
                  "SINEX: %s "
                  "(traceback: %s)\n",
                  vel, sta, site->site_code(), site->point_code(),
                  m_filename.c_str(), __func__);
          return 1;
        }
      } else if (std::strcmp(xit->soln_id(), vit->soln_id())) {
        /* solution IDs should be the same for X0 and Vx */
        fprintf(stderr,
                "[ERROR] Solution IDs for position and velocity do not match! "
                "Got \'%s\' vs \'%s\' for site %s, %s, components %s, %s. "
                "SINEX: %s (traceback: %s)\n",
                xit->soln_id(), vit->soln_id(), site->site_code(),
                site->point_code(), sta, vel, m_filename.c_str(), __func__);
        return 1;
      } else if (solnid[0] && (std::strcmp(xit->soln_id(), solnid))) {
        /* if we already have a solution id (e.g. from X compoenent) it should
         *  stay the same here (e.g. for Y component).
         */
        fprintf(stderr,
                "[ERROR] Solution IDs do not match for different components! "
                "Got \'%s\' vs \'%s\' for site %s, %s, components %s, %s. "
                "SINEX: %s (traceback: %s)\n",
                xit->soln_id(), solnid, site->site_code(), site->point_code(),
                sta, vel, m_filename.c_str(), __func__);
      } else {
        /* ok, we have coordinates and velocity for component */
        const double x0 = xit->estimate();
        const double vx = vit->estimate();
        const auto dt =
            t.diff<dso::DateTimeDifferenceType::FractionalYears>(xit->epoch());
        auto val = x0 + vx * dt.years();
        xyz[xcomponent / 2] = val;
        std::strcpy(solnid, xit->soln_id());
      }
    } /* end looping components for the site */

    /* did we collect all components ? */
    if ((xyz[0] == EMPTY) || (xyz[1] == EMPTY) || (xyz[2] == EMPTY)) {
      fprintf(stderr,
              "[ERROR] Failed retrieving components for site %s %s; "
              "SINEX: %s "
              "(traceback: %s)\n",
              site->site_code(), site->point_code(), m_filename.c_str(),
              __func__);
      return 1;
    }

    /* append to result vector */
    crd.emplace_back(dso::Sinex::SiteCoordinateResults(*site, solnid, xyz[0],
                                                       xyz[1], xyz[2]));
  } /* end looping sites */

  return 0;
}
