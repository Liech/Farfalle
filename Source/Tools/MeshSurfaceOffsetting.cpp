#include "MeshSurfaceOffsetting.h"

#include <set>
#include <stdexcept>
#include <cmath>
#include <glm/gtx/closest_point.hpp>

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

void MeshSurfaceOffsetting::subdivideTriangle(size_t faceID) {
  auto& face = faces[faceID];
  size_t a = face[0];
  size_t b = face[1];
  size_t c = face[2];
  
  glm::dvec3 center = (points[a] + points[b] + points[c]) / 2.0;
  points.push_back(center);
  border.push_back(false);
  size_t d = points.size() - 1;
  std::array<size_t, 3> faceA = { a, d,b };
  std::array<size_t, 3> faceB = { b, d,c };
  std::array<size_t, 3> faceC = { c, a,d };

  faces[faceID] = faceA;
  faces.push_back(faceB);
  faces.push_back(faceC);
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
  for (size_t faceID = 0; faceID < faces.size(); faceID++) {
    auto& face = faces[faceID];
    std::vector<double> distances = { distancesToBorder[face[0]],distancesToBorder[face[1]],distancesToBorder[face[2]] };
    int amountInfinity = (std::isinf(distances[0]) ? 1 : 0) + (std::isinf(distances[1]) ? 1 : 0) + (std::isinf(distances[2]) ? 1 : 0);
    if (amountInfinity > 1)
      continue;

    size_t subjectToChange;
    size_t A;
    size_t B;
    if (distances[0] > distances[1] && distances[0] > distances[2]) {
      subjectToChange = face[0];
      A = face[1];
      B = face[2];
    }
    else if (distances[1] > distances[0] && distances[1] > distances[2]) {
      subjectToChange = face[1];
      A = face[2];
      B = face[0];
    }
    else if (distances[2] > distances[1] && distances[2] > distances[0]) {
      subjectToChange = face[2];
      A = face[0];
      B = face[1];
    }

    glm::dvec3 onSegmentPoint = glm::closestPointOnLine(points[subjectToChange], points[A], points[B]);
    double contenderDistance = glm::distance(points[subjectToChange], onSegmentPoint);
    if (contenderDistance < distancesToBorder[subjectToChange])
      distancesToBorder[subjectToChange] = contenderDistance;
  }
}

void MeshSurfaceOffsetting::fillNeighbours() {

}

std::vector<std::pair<size_t, char>> MeshSurfaceOffsetting::getIsoEdges(double distance) const {
  std::vector<std::pair<size_t, char>> result;

  return result;
}