#include "LineSimplification.h"

#include <psimpl/psimpl.h>
#include <iterator>

std::vector<glm::dvec3> LineSimplification::simplify(const std::vector<glm::dvec3>& input, double tolerance) {

  std::vector <double> polyline;
  polyline.reserve(input.size() * 3);
  for (auto& x : input) for (int i = 0; i < 3; i++) 
    polyline.push_back(x[i]);
  
  std::vector <double> result;   // resulting simplified polyline
  
  // simplify the 2d polyline
  psimpl::simplify_radial_distance <3>(
    polyline.begin(), polyline.end(),
    tolerance, std::back_inserter(result));
  
  std::vector<glm::dvec3> vectorResult;
  vectorResult.resize(result.size() / 3);
  for (size_t i = 0; i < result.size(); i += 3)
    vectorResult.push_back(glm::dvec3(result[i], result[i+1], result[i+2]));
  
  return vectorResult;
}