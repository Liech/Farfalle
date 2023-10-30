#pragma once

#include <vector>
#include <functional>
#include <glm/glm.hpp>

//Constructive Solid Geometry
class CSG {
public:
  static void andInplace(const std::vector<bool>& a, std::vector<bool>& b);
  static void orInplace(const std::vector<bool>& a, std::vector<bool>& b);
  static void subtractInplace(const std::vector<bool>& a, std::vector<bool>& b);
  static void apply(const std::vector<bool>& a, std::vector<bool>& b, std::function<bool(bool, bool)> fun);
  static void apply(std::vector<double>& a, std::function<double(int, int, int)> fun, const glm::ivec3& resolution);
  static void applySingleCore(std::vector<double>& a, std::function<double(int, int, int)> fun, const glm::ivec3& resolution);
};