#include "MeshSurfaceOffsetting.h"

#include <set>
#include <stdexcept>
#include <cmath>

MeshSurfaceOffsetting::MeshSurfaceOffsetting(const std::vector<glm::dvec3>& points_, const std::vector<std::array<size_t, 3>>& faces_) {
  points = points_;
  faces  = faces_;
  initialize();
}

void MeshSurfaceOffsetting::initialize() {
  fillBorder();
  auto borderTris = getPureBorderTriangles();
  for (auto& tri : borderTris) 
    subdivideTriangle(tri);
  initializeDistances();
  while (hasInfiniteDistances())
    spreadDistance();
  fillNeighbours();
}

std::vector<std::vector<glm::dvec3>> MeshSurfaceOffsetting::offset(double distance) const {
  std::vector<std::vector<glm::dvec3>> result;
  std::vector<std::pair<size_t, char>> edges = getIsoEdges(distance);

  std::set<std::pair<size_t, char>> allEdges(edges.begin(), edges.end());
  std::set<std::pair<size_t, char>> unprocessed(allEdges);
  auto traceLoop = [&unprocessed](const std::pair<size_t, char>& currentEdge) {
    std::vector<glm::dvec3> result;
    unprocessed.erase(currentEdge);
    throw std::runtime_error("Not yet implemented");
    return result;
  };

  while (!unprocessed.empty()) {
    auto current = *unprocessed.begin();
    auto loop = traceLoop(current);
    result.push_back(loop);
  }

  return result;
}

void MeshSurfaceOffsetting::fillBorder(){

}

std::vector<size_t> MeshSurfaceOffsetting::getPureBorderTriangles() {
  std::vector<size_t> result;

  for (size_t i = 0; i < faces.size();i++) {
    std::array<size_t, 3>& x = faces[i];
    if (border[x[0]] && border[x[1]] && border[x[2]])
      result.push_back(i);
  }

  return result;
}

void MeshSurfaceOffsetting::subdivideTriangle(size_t) {

}

void MeshSurfaceOffsetting::initializeDistances() {
  distancesToBorder.resize(points.size());
  for (size_t i = 0; i < points.size(); i++) {
    if (border[i])
      distancesToBorder[i] = 0;
    else
      distancesToBorder[i] = std::numeric_limits<double>::infinity();
  }
}

bool MeshSurfaceOffsetting::hasInfiniteDistances() {
  for (size_t i = 0; i < points.size(); i++)
    if (std::isinf(distancesToBorder[i]))
      return true;
  return false;
}

void MeshSurfaceOffsetting::spreadDistance() {

}

void MeshSurfaceOffsetting::fillNeighbours() {

}

std::vector<std::pair<size_t, char>> MeshSurfaceOffsetting::getIsoEdges(double distance) const {
  std::vector<std::pair<size_t, char>> result;

  return result;
}