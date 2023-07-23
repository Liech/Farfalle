#include "InfillGenerator.h"


InfillGenerator::InfillGenerator(const Mesh2D& t) : target(t) {

}

void InfillGenerator::setSliceDirection(const glm::dvec2& dir) {
  sliceDirection = dir;
}

void InfillGenerator::setSliceDistance(double distance) {
  sliceDistance = distance;
}

void InfillGenerator::setLineWidth(double width) {
  lineWidth = width;
}

//returns path to fill out the Mesh2D with a printer of diameter sliceDistance
std::vector<std::vector<glm::dvec2>> InfillGenerator::generateInfill() const {
  std::vector<std::vector<glm::dvec2>> result;

  return result;
}

//generates all intersections at the 2d slice points
//usually result[i] is a concatenation of pairs that build lines inside the polygon
//edge case: Singular point on the edge
//edge case can be recognized by checking if the center of the line is inside the polygon
std::vector<std::vector<glm::dvec2>> InfillGenerator::calculateEggSliceIntersections() const {
  std::vector<std::vector<glm::dvec2>> result;

  return result;
}

//point of poly that is farthest placed into direction of "north"
//e.g. if north is -Xaxis it would be a point with: point.x < AllOther.x 
glm::dvec2 InfillGenerator::getPolePoint(const glm::dvec2 north) const {
  return glm::dvec2(0, 0);
}