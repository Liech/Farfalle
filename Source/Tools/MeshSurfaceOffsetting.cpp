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
  fillGraphs();
  fillBorder();
  auto borderTris = getPureBorderTriangles();
  for (auto& tri : borderTris) 
    subdivideTriangle(tri);
  if (borderTris.size() != 0)
    fillGraphs();
  fillDistances();
  while (hasInfiniteDistances())
    spreadDistance();
}

MeshSurfaceOffsetting::Result MeshSurfaceOffsetting::offset(double isovalue) const {
  MeshSurfaceOffsetting::Result result;
  std::set<std::pair<size_t, char>> edges = getIsoEdges(isovalue);
  std::set<std::pair<size_t, char>> unprocessed(edges);

  while (!unprocessed.empty()) {
    auto current = *unprocessed.begin();
    auto loop = traceLoop(current, unprocessed, edges, isovalue);
    if (loop.first)
      result.closed.push_back(loop.second);
    else
      result.open.push_back(loop.second);
  }

  return result;
}

void MeshSurfaceOffsetting::fillGraphs() {
  std::map<std::pair<size_t, size_t>, size_t> edges;
  std::map<std::pair<size_t, size_t>, std::pair<size_t, char>> pointPoint2EdgeID;

  neighbours.clear();
  //count how often each edge occurs
  for (size_t faceID = 0; faceID < faces.size(); faceID++) {
    auto& face = faces[faceID];
    for (int i = 0; i < 3; i++) {
      size_t a = face[(i + 0) % 3];
      size_t b = face[(i + 1) % 3];
      if (b < a)
        std::swap(a, b);
      auto edge = std::make_pair(a, b);
      if (edges.count(edge) == 0) {
        edges[edge] = 1;
        pointPoint2EdgeID[edge] = std::make_pair(faceID, i);
      }
      else {
        edges[edge]++;
        auto a = pointPoint2EdgeID[edge];
        auto b = std::make_pair(faceID, i);
        neighbours[a] = b;
        neighbours[b] = a;
      }
    }
  }

  borderEdges.clear();
  //edges with occurence of 1 are border edges
  for (auto& x : edges) {
    if (x.second == 1) {
      auto& id = pointPoint2EdgeID[x.first];
      borderEdges.insert(id);
    }
  }
}

void MeshSurfaceOffsetting::fillBorder(){
  border.resize(points.size());
  for (size_t i = 0; i < border.size(); i++)
    border[i] = false;

  for (auto& edge : borderEdges) {
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

void MeshSurfaceOffsetting::fillDistances() {
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


std::set<std::pair<size_t, char>> MeshSurfaceOffsetting::getIsoEdges(double isovalue) const {
  std::set<std::pair<size_t, char>> result;

  for (size_t faceID = 0; faceID < faces.size(); faceID++) {
    auto& face = faces[faceID];
    for (int i = 0; i < 3; i++) {
      size_t a = face[(i + 0) % 3];
      size_t b = face[(i + 1) % 3];
      if (distancesToBorder[b] < distancesToBorder[a])
        std::swap(a, b);
      if (distancesToBorder[a] < isovalue && distancesToBorder[b] > isovalue)
        result.insert(std::make_pair(faceID, i));
    }
  }

  return result;
}

std::pair<size_t, char> MeshSurfaceOffsetting::getNextEdge(const std::pair<size_t, char>& current, const std::set<std::pair<size_t, char>>& isoEdges) const {
  std::pair<size_t, char> currentNeighbour = neighbours.at(current);
  std::pair<size_t, char> candiateA = std::make_pair(currentNeighbour.first, (currentNeighbour.second + 1) % 3);//other edges of the face
  std::pair<size_t, char> candiateB = std::make_pair(currentNeighbour.first, (currentNeighbour.second + 2) % 3);
  bool aPossible = isoEdges.count(candiateA) != 0;
  bool bPossible = isoEdges.count(candiateB) != 0;
  assert(!(aPossible && bPossible));

  if (aPossible)
    return candiateA;
  else if (bPossible)
    return candiateB;
  else 
    return std::make_pair(std::numeric_limits<size_t>::max(), std::numeric_limits<char>::max()); //no next found
}

std::pair<size_t, char> MeshSurfaceOffsetting::getPreviousEdge(const std::pair<size_t, char>& current, const std::set<std::pair<size_t, char>>& isoEdges) const {
  std::pair<size_t, char> candiateA = std::make_pair(current.first, (current.second + 1) % 3);//other edges of the face
  std::pair<size_t, char> candiateB = std::make_pair(current.first, (current.second + 2) % 3);
  bool aPossible = isoEdges.count(candiateA) != 0;
  bool bPossible = isoEdges.count(candiateB) != 0;

  if (aPossible)
    return neighbours.at(candiateA);
  else if (bPossible)
    return neighbours.at(candiateB);
  else
    return std::make_pair(std::numeric_limits<size_t>::max(), std::numeric_limits<char>::max()); //no previous found
}

glm::dvec3 MeshSurfaceOffsetting::interpolate(const std::pair<size_t, char>& edge, double isovalue) const {
  auto face = faces[edge.first];
  size_t A = face[edge.second];
  size_t B = face[(edge.second+1)%3];

  glm::dvec3 pointA    = points[A];
  glm::dvec3 pointB    = points[B];
  double     distanceA = distancesToBorder[A];
  double     distanceB = distancesToBorder[B];

  double perc = (isovalue - distanceA) / (distanceB - distanceA);
  glm::dvec3 result = pointA * (perc-1) + pointB * perc;
  return result;
}

std::pair<bool, std::vector<glm::dvec3>> MeshSurfaceOffsetting::traceLoop(const std::pair<size_t, char>& seed, std::set<std::pair<size_t, char>>& unprocessed, const std::set<std::pair<size_t, char>>& allIsoEdges, double isovalue) const {
  bool                    result_isClosed = false;
  std::vector<glm::dvec3> result_loop;
  
  std::pair<size_t, char> current = seed;
  while (true) {
    unprocessed.erase(current);
    auto point = interpolate(current, isovalue);
    result_loop.push_back(point);
    auto next = getNextEdge(current, allIsoEdges);
    if (next.second == std::numeric_limits<char>::max())
      break;
    if (next == seed) {
      result_isClosed = true;
      break;
    }
  }

  if (!result_isClosed) {
    std::reverse(result_loop.begin(),result_loop.end());
    result_loop.erase(result_loop.begin() + result_loop.size() - 1); //will be readded
    current = seed;
    while (true) {
      unprocessed.erase(current);
      auto point = interpolate(current, isovalue);
      result_loop.push_back(point);
      auto next = getPreviousEdge(current, allIsoEdges);
      if (next.second == std::numeric_limits<char>::max())
        break;
    }
    std::reverse(result_loop.begin(), result_loop.end());
  }

  return std::make_pair(result_isClosed,result_loop);
}