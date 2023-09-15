#include "sinex.hpp"
#include <algorithm>

int dso::filter_solution_estimates(
    const std::vector<dso::sinex::SolutionEstimate> &estimates,
    const std::vector<dso::sinex::SolutionEpoch> &epochs,
    std::vector<dso::sinex::SolutionEstimate> &filtered_estimates) noexcept {

  /* clear output vector and allocate memory */
  if (!filtered_estimates.empty())
    filtered_estimates.clear();
  if (filtered_estimates.size() < estimates.size())
    filtered_estimates.reserve(estimates.size());

  /* filter entries in estimates and place them in filtered_estimates */
  std::copy_if(
      estimates.begin(), estimates.end(),
      std::back_inserter(filtered_estimates),
      [&](const dso::sinex::SolutionEstimate &estimate) {
        return std::find_if(
                   epochs.begin(), epochs.end(),
                   [&](const dso::sinex::SolutionEpoch &epoch) {
                     return (estimate.site_code() == epoch.site_code()) &&
                            (estimate.point_code() == epoch.point_code()) &&
                            (estimate.soln_id() == epoch.soln_id());
                   }) != epochs.end();
      });

  return 0;
}
