#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <glm/glm.hpp>

class SliceConfig;
class Model;

//Converts Slices of a model into Line Streaks by Slicing
class Spaghettifyer {
public:
  Spaghettifyer(Model& mainModel,const SliceConfig&);
  virtual ~Spaghettifyer();

  std::vector<std::vector<glm::dvec3>> spaghettify(Model&) const;
private:
  std::vector<std::unique_ptr<Model>> tools;
};