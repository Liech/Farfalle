#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <set>
#include <array>

class MeshSurfaceOffsetting {
public:
  MeshSurfaceOffsetting(const std::vector<glm::dvec3>& points, const std::vector<std::array<size_t, 3>>& faces);

  struct Result {
    std::vector<std::vector<glm::dvec3>> closed;
    std::vector<std::vector<glm::dvec3>> open;
  };
  MeshSurfaceOffsetting::Result offset(double distance) const; //returns loop soup
private:
  void initialize();
  void fillGraphs(); //fills neighbours and borderEdges
  void fillBorder(); //fill border
  std::vector<size_t> getPureBorderTriangles();
  void subdivideTriangle(size_t);
  void fillDistances(); //fill distancesToBorder
  bool hasInfiniteDistances();
  void spreadDistance(); //spreads from border into the inner and removes infinit distances
  std::set<std::pair<size_t, char>> getIsoEdges(double distance) const; //result contains edge twices (as of different faces)
  std::pair<size_t, char> getNextEdge(const std::pair<size_t, char>& current, const std::set<std::pair<size_t, char>>& isoEdges) const;     //char == 4 on Not found
  std::pair<size_t, char> getPreviousEdge(const std::pair<size_t, char>& current, const std::set<std::pair<size_t, char>>& isoEdges) const; //char == 4 on Not found 
  std::pair<bool, std::vector<glm::dvec3>> traceLoop(const std::pair<size_t, char>& seed, std::set<std::pair<size_t, char>>& unprocessed, const std::set<std::pair<size_t, char>>& all, double isovalue) const;
  glm::dvec3 interpolate(const std::pair<size_t, char>& edge, double isovalue) const;

  std::vector<glm::dvec3>                                     points; //input (may be changed only by subdivideTriangle)
  std::vector<std::array<size_t, 3>>                          faces;  //input (may be changed only by subdivideTriangle)
  std::vector<bool>                                           border; //size=points.size
  std::vector<double>                                         distancesToBorder; //size=points.size; Initialized with 0 or infinityz
  std::map< std::pair<size_t, char>, std::pair<size_t, char>> neighbours;
  std::set<std::pair<size_t, char>>                           borderEdges;
};