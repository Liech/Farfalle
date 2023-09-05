#include "Macaronifyer.h"

#include <iostream>

#include "Tools/LineSimplification.h"
#include "SliceConfig.h"
#include "Model.h"

Macaronifyer::Macaronifyer(const SliceConfig& config) {

}

Macaronifyer::~Macaronifyer() {

}

std::vector<std::vector<glm::dvec3>> Macaronifyer::macaronify(Model& toNoodlize) const {
  std::vector<std::vector<glm::dvec3>> result;
  result = toNoodlize.getBorder();
  

  //wrap around:
  for (int i = 0; i < result.size(); i++) {
    result[i].push_back(result[i][0]);
  }
  return result;
}
