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
  border.resize(points.size());
  for (size_t i = 0; i < border.size(); i++)
    border[i] = false;

  auto edges = getBorderEdges();
  for (auto& edge : edges) {
    auto& tri = faces[edge.first];
    if (edge.second == 0) {
      border[tri[0]] = true;
      border[tri[1]] = true;
    }
    if (edge.second == 1) {
      border[tri[1]] = true;
      border[tri[2]] = true;
    }
    if (edge.second == 2) {
      border[tri[2]] = true;
      border[tri[0]] = true;
    }
  }
}

std::set<std::pair<size_t, char>> MeshSurfaceOffsetting::getBorderEdges() {
  std::set<std::pair<size_t, char>> result;
  std::map<std::pair<size_t, size_t>, size_t> edges;
  std::map<std::pair<size_t, size_t>, std::pair<size_t, char>> pointPoint2EdgeID;

  //count how often each edge occurs
  for (size_t faceID = 0; faceID < faces.size();faceID++) {
    auto& face = faces[faceID];
    for (int i = 0; i < 3; i++) {
      int a = face[(i + 0) % 3];
      int b = face[(i + 1) % 3];
      if (b < a) 
        std::swap(a, b);
      auto edge = std::make_pair(a, b);
      if (edges.count(edge) == 0) {
        edges[edge] = 1;
        pointPoint2EdgeID[edge] = std::make_pair(faceID, i);
      }
      else
        edges[edge]++;
    }
  }

  //edges with occurence of 1 are border edges
  for (auto& x : edges) {
    if (x.second == 1) {
      auto& id = pointPoint2EdgeID[x.first];
      result.insert(id);
    }
  }
  
  return result;
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