#pragma once

#include <vector>
#include <glm/glm.hpp>

class Mesh2D;

//Egg Slicing Infill generator
//https://stackoverflow.com/a/15767638
class InfillGenerator {
public:
  InfillGenerator(Mesh2D&);

  void setSliceDirection(const glm::dvec2& direction);
  void setSliceDistance(double distance);
  void setLineWidth    (double width);

  std::vector<std::vector<glm::dvec2>> generateInfill() const;

private:
  static glm::dvec2 getClosestPointOnInfiniteLine(const glm::dvec2 point, const glm::dvec2 start, const glm::dvec2 end);
  std::vector<std::vector<glm::dvec2>> calculateEggSliceIntersections() const;


  Mesh2D& target;

  glm::dvec2 sliceDirection = glm::dvec2 (0,1); //orientation of infill
  double     sliceDistance = 0.4; //spacing of the infill lines
  double     lineWidth     = 0.4; //important on the borders
};