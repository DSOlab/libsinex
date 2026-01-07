#include "dpod.hpp"

int dso::dpod_extrapolate(const dso::datetime<dso::nanoseconds> &t,
                          const std::vector<const char *> &sites_4charid,
                          std::vector<dso::Sinex::SiteCoordinateResults> &crd,
                          const char *dpod_snx, const char *dpod_freq,
                          bool apply_eccentricities) noexcept {
  /* create the sinex instance */
  dso::Sinex snx(dpod_snx);

  /* a vector of SiteId's to hold (intermediate) results */
  std::vector<dso::sinex::SiteId> siteids;

  /* parse the block SITE/ID to collect info for the given sites */
  if (snx.parse_block_site_id(sites_4charid, /*use domes=*/false, siteids)) {
    fprintf(stderr, "ERROR. Failed matching sites in SINEX file\n");
    return 1;
  }

  /* extrapolate coordinates (linear model) -> crd */
  if (snx.linear_extrapolate_coordinates(siteids, t, crd)) {
    fprintf(stderr, "ERROR Failed extrapolating coordinate estimates\n");
    return 1;
  }

  /* append harmonics signal(s) -> crd */
  if (dpod_freq) {
    if (dso::apply_dpod_freq_corr(dpod_freq, t, crd)) {
      fprintf(stderr,
              "[ERROR] Failed applying dpod frequency corrections; file is %s "
              "(traceback: %s)\n",
              dpod_freq, __func__);
      return 1;
    }
  }

  /* apply eccentricities if needed */
  if (apply_eccentricities) {
    /* store parsed eccentricities here */
    std::vector<dso::sinex::SiteEccentricity> ecc;
    /* get the eccentricities */
    if (snx.parse_block_site_eccentricity(siteids, t, ecc)) {
      fprintf(stderr, "Failed collecting site eccentricities\n");
      return 1;
    }
    /* apply, iterativelly, for every site in the crd vector */
    for (auto sb = crd.begin(); sb != crd.end(); ++sb) {
      /* find eccentricity for this site (aka sb) in the ecc vector */
      auto it = std::find_if(
          ecc.begin(), ecc.end(), [sb](const dso::sinex::SiteEccentricity &se) {
            return (
                ((!std::strcmp(se.site_code(), sb->msite.site_code())) &&
                 (!std::strcmp(se.point_code(), sb->msite.point_code()))) &&
                (se.soln_id_int() == sb->soln_id_int()));
          });
      if (it == ecc.end()) {
        fprintf(stderr,
                "[ERROR] Failed finding eccentricity for site %s (%s) SOLN %s "
                "(traceback: %s)\n",
                sb->msite.site_code(), sb->msite.point_code(), sb->soln_id(),
                __func__);
        return 1;
      }
      /* apply eccentricity depending on referrence system */
      Eigen::Vector3d dr = Eigen::Vector3d::Zero();
      if (!std::strcmp(it->ref_system(), "XYZ")) {
        dr << it->eccentricity(0), it->eccentricity(1), it->eccentricity(2);
      } else if (!std::strcmp(it->ref_system(), "UNE")) {
        /* rotate eccentricity to XYZ :
         * rotation matrix R: (dX,dY,dZ) = R * (e,n,u)
         */
        const Eigen::Matrix3d R =
            dso::lvlh(dso::CartesianCrdConstView(sb->cartesian_crd()));
        Eigen::Vector3d enu;
        enu << it->eccentricity(2), it->eccentricity(1), it->eccentricity(0);
        dr = R * enu;
      } else {
        fprintf(stderr,
                "[ERROR] Invalid reference system in eccentricity! System is: "
                "[%s] (traceback: %s)\n",
                it->ref_system(), __func__);
        return 1;
      }
      sb->x += dr(0);
      sb->y += dr(1);
      sb->z += dr(2);
    }
  } /* apply eccentricities */

  return 0;
}
