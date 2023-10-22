#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <memory>

class KDTreePImpl;

class KDTree {
public:
  KDTree(const std::vector<glm::dvec3>&);
  virtual ~KDTree();

  std::vector<size_t> find(const glm::dvec3&, double radius);
  void   remove(size_t d);
private:
  std::unique_ptr<KDTreePImpl> p; //cgal kd tree
};