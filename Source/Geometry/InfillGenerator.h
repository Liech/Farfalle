#pragma once

#include <vector>
#include <glm/glm.hpp>

class Mesh2D;

//Egg Slicing Infill generator
//https://stackoverflow.com/a/15767638
class InfillGenerator {
public:
  InfillGenerator(const Mesh2D&);

  void setSliceDirection(const glm::dvec2& direction);
  void setSliceDistance(double distance);
  void setLineWidth    (double width);

  std::vector<std::vector<glm::dvec2>> generateInfill() const;

private:
  glm::dvec2 getPolePoint(const glm::dvec2 north) const; 
  std::vector<std::vector<glm::dvec2>> calculateEggSliceIntersections() const;


  const Mesh2D& target;

  glm::dvec2 sliceDirection = glm::dvec2 (0,1); //orientation of infill
  double     sliceDistance = 0.4; //spacing of the infill lines
  double     lineWidth     = 0.4; //important on the borders
};