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
    printf("\tSite %s/%s Domes %s\n", s.m_site_code, s.m_point_code, s.m_domes);
  printf("Parsing block sites returned %d\n", error);

  // Ok, now we have all the info we need, for the sites we want estimates for
  // get the estimates
  // first declare a vector of SolutionEstimate to hold results
  std::vector<dso::sinex::SolutionEstimate> est_vec;
  error = snx.parse_block_solution_estimate(est_vec, site_vec);
  printf("Number of sites collected: %lu\n", est_vec.size());
  for (auto const &e : est_vec)
    printf("\tParameter: [%s] Site: [%s] Estimate %.5f +/- %.5f\n",
           e.m_param_type, e.m_site_code, e.m_estimate, e.m_std_deviation);
  printf("Parsing estimates returned %d\n", error);

  // de-allocate memory
  for (int i=0; i<num_sites; i++) delete[] sites[i];
  delete[] sites;

  return 0;
}
