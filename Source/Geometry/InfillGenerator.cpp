#include "InfillGenerator.h"

#include <glm/gtx/closest_point.hpp>

#include "Mesh2D.h"

InfillGenerator::InfillGenerator(Mesh2D& t) : target(t) {

}

void InfillGenerator::setSliceDirection(const glm::dvec2& dir) {
  sliceDirection = glm::normalize(dir);
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
  //glm::dvec2 sliceStart = getPolePoint(sliceDirection);
  //glm::dvec2 sliceEnd   = getPolePoint(-sliceDirection);
  //
  //sliceEnd  = getClosestPointOnInfiniteLine(sliceStart, sliceEnd, sliceEnd + sliceDirection);
  //
  //glm::dvec2 orthogonal = glm::normalize(sliceStart - sliceEnd);
  //double distance  = glm::distance(sliceStart, sliceEnd);
  //double increment = sliceDistance / distance;

  //for (double location = 0; location < distance; location += increment) {
  //  glm::dvec2 position = sliceStart + orthogonal * location;

  //  std::vector<glm::dvec2> intersections = target.intersectLine(position, sliceDirection);
  //  result.push_back(intersections);
  //}

  return result;
}

//copied from
//glm::closestPointOnLine(sliceStart, sliceEnd, sliceEnd + sliceDirection);
//to function with lines instead of line segments
glm::dvec2 InfillGenerator::getClosestPointOnInfiniteLine(const glm::dvec2 point, const glm::dvec2 a, const glm::dvec2 b){
  double LineLength = glm::distance(a, b);
  glm::dvec2 Vector = point - a;
  glm::dvec2 LineDirection = (b - a) / LineLength;

  // Project Vector to LineDirection to get the distance of point from a
  double Distance = dot(Vector, LineDirection);

  // /*removal here is difference to original*/if (Distance <= 0) return a;
  // /*removal here is difference to original*/if (Distance >= LineLength) return b;
  return a + LineDirection * Distance;
}