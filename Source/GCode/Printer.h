#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>



class Extruder;
class Temperature;
class Movement;

class Printer
{
public:

  glm::dvec3 dimensions = glm::dvec3(200, 200, 200); //mm

  std::unique_ptr<Extruder>    extruder;
  std::unique_ptr<Temperature> temperature;
  std::unique_ptr<Movement>    movement;

  Printer();
  ~Printer() = default;

  void startup (std::string&);
  void shutdown(std::string&);
};