#pragma once

#include <vector>
#include <functional>

//Constructive Solid Geometry
class CSG {
public:
  static void andInplace(const std::vector<bool>& a, std::vector<bool>& b);
  static void orInplace(const std::vector<bool>& a, std::vector<bool>& b);
  static void subtractInplace(const std::vector<bool>& a, std::vector<bool>& b);
  static void apply(const std::vector<bool>& a, std::vector<bool>& b, std::function<bool(bool, bool)> fun);
};