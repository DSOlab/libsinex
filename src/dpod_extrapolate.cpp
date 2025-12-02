#include "utilities.hpp"

int dso::dpod_extrapolate(const dso::datetime<dso::nanoseconds> &t, const std::vector<const char *> &sites_4charid, const char *dpod_snx) noexcept {
  /* create the sinex instance */
  dso::Sinex snx(dpod_snx);

  /* a vector of SiteId's to hold (intermediate) results */
  std::vector<dso::sinex::SiteId> siteids;

  /* parse the block SITE/ID to collect info for the given sites */
  if (snx.parse_block_site_id(sites_4charid, /*use domes=*/false, siteids)) {
    fprintf(stderr, "ERROR. Failed matching sites in SINEX file\n");
    return 1;
  }

  /* hold results here */
  std::vector<dso::Sinex::SiteCoordinateResults> crd;

  /* filter estimates based on date */
  if (snx.linear_extrapolate_coordinates(siteids, t, crd)) {
    fprintf(stderr, "ERROR Failed extrapolating coordinate estimates\n");
    return 1;
  }

  return 0;
}
