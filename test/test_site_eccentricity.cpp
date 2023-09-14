#include "sinex.hpp"
#include <iostream>

int main(int argc, char *argv[]) {

  if (argc < 3) {
    fprintf(stderr, "Usage: %s [SINEX_FILE] [SITE CODE1 ... SITE CODEN]\n",
            argv[0]);
    return 1;
  }

  // number of sites at input
  int num_sites = argc - 2;
  printf("Number of sites to process: %d\n", num_sites);

  // create an array of sites ...
  char **sites = new char *[num_sites];
  for (int i = 0; i < num_sites; i++) {
    sites[i] = new char[5];
    std::memset(sites[i], 0, 5);
    std::strcpy(sites[i], argv[i + 2]);
  }
  printf("Searching SINEX file for the following sites:\n");
  for (int i = 0; i < num_sites; i++)
    printf("\t%s\n", sites[i]);

  // create the sinex instance
  dso::Sinex snx(argv[1]);

  // a vector of SiteId to hold results
  std::vector<dso::sinex::SiteId> site_vec;

  // parse the block SITE/ID to collect info for the given sites
  int error = snx.parse_block_site_id(site_vec, num_sites, sites);

  // print status
  printf("Number of sites collected: %lu\n", site_vec.size());
  for (auto const &s : site_vec)
    printf("\tSite %s/%s Domes %s\n", s.site_code(), s.point_code(), s.domes());
  printf("Parsing block sites returned %d\n", error);

  // get the eccentricities
  dso::datetime<dso::seconds> t(dso::year(2020), dso::month(1),
                                dso::day_of_month(1), dso::seconds(0));
  std::vector<dso::sinex::SiteEccentricity> ecc;
  if (snx.parse_block_site_eccentricity(ecc, t, site_vec)) {
    fprintf(stderr, "Failed collecting site eccentricities\n");
    return 1;
  }

  printf("Eccentricity per site:\n");
  for (const auto &e : ecc) {
    printf("%s %.6f %.6f %.6f\n", e.soln_id(), e.une[0], e.une[1], e.une[2]);
  }

  printf("All seem ok!\n");
  return 0;
}
