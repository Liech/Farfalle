#include "Spaghettifyer.h"

#include <iostream>

#include "Tools/LineSimplification.h"
#include "SliceConfig.h"
#include "Model.h"

Spaghettifyer::Spaghettifyer(Model& mainModel, const SliceConfig& config){
  double offset = mainModel.getMin()[0];
  size_t size = (mainModel.getMax()[0]- mainModel.getMin()[0]) / config.layerWidth;
  for (int i = 0; i < size; i++) {
    std::cout << "Spagetthifyer Tool Generation: " << i << "/" << size << std::endl;
    double x = i * config.layerWidth;

    std::function<double(const glm::dvec3& point)> sphereFun = [x, &config](const glm::dvec3& point) {
      if (point.z < config.CenterPoint.z) {
        if (point.z > config.CenterPoint.z - config.BoundingSphereRadius / 2.0) {
          if ((glm::distance(glm::dvec3(point.x, point.y, 0), glm::dvec3(config.CenterPoint.x, config.CenterPoint.y, 0)) - x) < 0)
            return -1.0;
          else
            return 1.0;
        }
        else
          return -1.0;
      }

      return glm::distance(point, config.CenterPoint) - x;
    };

    std::function<double(const glm::dvec3& point)> fun = [offset, x](const glm::dvec3& point) {
      return point.x - (x + offset);
    };
    auto tool = std::make_unique<Model>(fun, config.CenterPoint, config.BoundingSphereRadius*2, config.Precision);
    tool->save("dbg/Spaghettifyer" + std::to_string(i) + ".stl");
    tools.push_back(std::move(tool));
  }
}

Spaghettifyer::~Spaghettifyer() {

}

std::vector<std::vector<glm::dvec3>> Spaghettifyer::spaghettify(Model& toNoodlize) const {
  std::vector<std::vector<glm::dvec3>> result;
  for (int i = 0; i < tools.size(); i++) {
    
    auto noodle = toNoodlize.slice2(*tools[i]);
    //for(int i = 0;i < noodle.size();i++)
    //    noodle[i] = LineSimplification::simplify(noodle[i]);
    result.insert(result.begin(), noodle.begin(), noodle.end());
  }
  return result;
}
