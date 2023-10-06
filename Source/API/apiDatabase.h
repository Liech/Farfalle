#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <glm/glm.hpp>

#include "GCode/Printer.h"
#include "GCode/Movement.h"
#include "GCode/Extruder.h"
#include "GCode/Temperature.h"

class Model;
class Printer;

class apiDatabase {
public:
  std::map<std::string, std::unique_ptr<std::vector<bool>>>                    voxel;
  std::map<std::string, std::unique_ptr<std::vector<double>>>                  volume;
  std::map<std::string, std::unique_ptr<Model>>                                models;
  std::map<std::string, std::unique_ptr<std::vector<std::vector<glm::dvec3>>>> lines;
  
  std::unique_ptr<Printer> printer;
};