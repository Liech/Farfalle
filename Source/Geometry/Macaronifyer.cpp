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
  return toNoodlize.getBorder();
    
  //return result;
}
