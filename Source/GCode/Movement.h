#pragma once

#include <string>
#include <glm/glm.hpp>

#include "Printer.h"
#include "Tools/MovementType.h"

class Movement {
public:
  glm::dvec3   currentPosition;
  glm::dvec2   restPosition;
  glm::dvec3   accelerationLimits    = glm::dvec3(2500, 2500, 400);
  glm::dvec3   maxFeedrate           = glm::dvec3(180, 180, 12);
  glm::dvec3   jerkLimit             = glm::dvec3(8, 8, 2);
  double       printAcceleration     = 2000;
  double       retractAcceleration   = 1250;
  double       travelAcceleration    = 2500;
  double       printSpeed            = 900;
  double       retractSpeed          = 4200;
  double       travelSpeed           = 9000;
  MovementType travelType            = MovementType::Custom;
  double       minimumTravelFeedRate = 0;
  bool         hasMeshBedLeveling    = true;

  Movement(Printer& printer);
  ~Movement() = default;

  void startup(std::string&);
  void home(std::string&);
  void shutdown(std::string&);

  void travelMode (std::string&);
  void printMode  (std::string&);
  void retractMode(std::string&);
  void customMode (std::string&, double speed);


  void moveTo(std::string&, const glm::dvec3&);
private:
  Printer& printer;
};