#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <array>

class MeshSurfaceOffsetting {
public:
  MeshSurfaceOffsetting(const std::vector<glm::dvec3>& points, const std::vector<std::array<size_t, 3>>& faces);

  std::vector<std::vector<glm::dvec3>> offset(double distance) const; //returns loop soup
private:
  void initialize();
  void fillBorder();
  std::vector<size_t> getPureBorderTriangles();
  void subdivideTriangle(size_t);
  void initializeDistances();
  bool hasInfiniteDistances();
  void spreadDistance(); //spreads from border into the inner and removes infinit distances
  void fillNeighbours();
  std::vector<std::pair<size_t, char>> getIsoEdges(double distance) const;

  std::vector<glm::dvec3>                   points; //input (may be changed only by subdivideTriangle)
  std::vector<std::array<size_t, 3>>        faces;  //input (may be changed only by subdivideTriangle)
  std::vector<bool>                         border; //size=points.size
  std::vector<double>                       distancesToBorder; //size=points.size; Initialized with 0 or infinity
  std::map<std::pair<size_t, char>, size_t> neighbours; //Triangle neighbours .second is edge ID

};