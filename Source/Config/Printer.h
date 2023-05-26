#pragma once

#include <string>
#include <glm/glm.hpp>

class Printer
{
public:

  glm::dvec3 dimensions                 = glm::dvec3(200, 200, 200); //mm
  glm::dvec3 currentPosition            = glm::dvec3(dimensions[0]/2,dimensions[1]/2,5);
  glm::dvec2 restPosition               = glm::dvec2(dimensions[0] * 0.8, dimensions[1] * 0.8);
  glm::dvec3 accelerationLimits         = glm::dvec3(2500, 2500, 400);
  double     extruderAccelerationLimits = 5000;
  glm::dvec3 maxFeedrate                = glm::dvec3(180, 180, 12);
  double     maxExtruderFeedrate        = 80;
  double     printAcceleration          = 2000;
  double     retractAcceleration        = 1250;
  double     travelAcceleration         = 2500;
  glm::dvec3 jerkLimit                  = glm::dvec3(8,8,2);
  double     extruderJerkLimit          = 10;
  double     minimumExtrudingFeedRate   = 0;
  double     minimumTravelFeedRate      = 0;
  bool       hasMeshBedLeveling         = true;
  double     flow                       = 95;
  double     linearAdvance              = 0.2;

  void startup (std::string&);
  void shutdown(std::string&);
  void home    (std::string&);
  void moveTo  (std::string&, const glm::dvec3&, double feedrate);
};