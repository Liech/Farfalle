#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <glm/glm.hpp>

class SliceConfig;
class Model;

//Converts Slices of a model into loops by minkowski erosion
class Macaronifyer {
public:
  Macaronifyer(Model& mainModel, const SliceConfig&);
  virtual ~Macaronifyer();

  std::vector<std::vector<glm::dvec3>> macaronify(Model&) const;
private:
  const SliceConfig& config;
  std::vector<std::unique_ptr<Model>> tools;
};