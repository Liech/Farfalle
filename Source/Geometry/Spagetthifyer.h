#pragma once

#include <vector>
#include <functional>
#include <glm/glm.hpp>

class SliceConfig;
class Model;

//Converts Slices of a model into Line Streaks by Slicing
class Spagetthifyer {
public:
  Spagetthifyer(Model& mainModel,const SliceConfig&);
  virtual ~Spagetthifyer();

  std::vector<std::vector<glm::dvec3>> spaghettify(Model&) const;
private:
  std::vector<std::unique_ptr<Model>> tools;
};