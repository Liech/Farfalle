#pragma once

#include <vector>
#include <functional>
#include <glm/glm.hpp>

class SliceConfig;
class Model;

//Converts Slices of a model into loops by minkowski erosion
class Macaronifyer {
public:
  Macaronifyer(const SliceConfig&);
  virtual ~Macaronifyer();

  std::vector<std::vector<glm::dvec3>> macaronify(Model&) const;
private:
};