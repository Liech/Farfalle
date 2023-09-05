#include "Spagetthifyer.h"

#include <iostream>

#include "Tools/LineSimplification.h"
#include "SliceConfig.h"
#include "Model.h"

Spagetthifyer::Spagetthifyer(Model& mainModel, const SliceConfig& config){
  double offset = mainModel.getMin()[0];
  size_t size = (mainModel.getMax()[0]- mainModel.getMin()[0]) / config.layerWidth;
  for (int i = 0; i < size; i++) {
    std::cout << "Spagetthifyer Tool Generation: " << i << "/" << size << std::endl;
    double x = i * config.layerWidth;
    std::function<double(const glm::dvec3& point)> fun = [offset, x](const glm::dvec3& point) {
      return point.x - (x + offset);
    };
    auto tool = std::make_unique<Model>(fun, config.CenterPoint, config.BoundingSphereRadius, config.Precision);
    //tool->save("dbg/Spagetthifyer" + std::to_string(i) + ".stl");
    tools.push_back(std::move(tool));
  }
}

Spagetthifyer::~Spagetthifyer() {

}

std::vector<std::vector<glm::dvec3>> Spagetthifyer::spagetthify(Model& toNoodlize) const {
  std::vector<std::vector<glm::dvec3>> result;
  for (int i = 0; i < tools.size(); i++) {
    auto noodle = toNoodlize.slice(*tools[i]);
    for(int i = 0;i < noodle.size();i++)
        noodle[i] = LineSimplification::simplify(noodle[i]);
    result.insert(result.begin(), noodle.begin(), noodle.end());
  }
  return result;
}
