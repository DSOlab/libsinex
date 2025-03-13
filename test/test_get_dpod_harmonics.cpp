#include "dpod.hpp"
#include "sinex.hpp"
#include <cstdio>

int main(int argc, char* argv[])
{
  if (argc < 4) {
    fprintf(stderr, "Usage: %s [SINEX_FILE] [dpod*_freq_corr.txt] [SITE1 ... SITEN]\n", argv[0]);
    return 2;
  }

  /* place sites in a vector */
  std::vector<const char*> sites;
  for (int i = 3; i < argc; i++) {
    sites.push_back(argv[i]);
  }

  /* create the sinex instance */
  dso::Sinex snx(argv[1]);

  /* a vector of SiteId's to hold (intermediate) results */
  std::vector<dso::sinex::SiteId> siteids;

  /* parse the block SITE/ID to collect info for the given sites */
  if (snx.parse_block_site_id(sites, /*use domes=*/false, siteids)) {
    fprintf(stderr, "ERROR. Failed matching sites in SINEX file\n");
    return 1;
  }

  std::vector<dso::SiteRealHarmonics> harm;

  if (dso::parse_dpod_freq_corr(argv[2], siteids, harm)) {
    fprintf(stderr, "ERROR. Failed parsing harmonics from %s\n", argv[2]);
    return 3;
  }

  for (const auto& h : harm) {
    for (int i = 0; i < h.harmonics().num_harmonics(); i++) {
      printf("%s %.2f %.3f %.3f\n", h.site_name(), *(h.harmonics().operator()(i) + 0), *(h.harmonics().operator()(i) + 2), *(h.harmonics().operator()(i) + 1));
    }
  }

  return 0;
}
