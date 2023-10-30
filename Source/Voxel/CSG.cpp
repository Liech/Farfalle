#include "CSG.h"

#include <cassert>

void CSG::andInplace(const std::vector<bool>& a, std::vector<bool>& b) {
  apply(a, b, [](bool a, bool b) {return a && b; });
}

void CSG::orInplace(const std::vector<bool>& a, std::vector<bool>& b) {
  apply(a, b, [](bool a, bool b) {return a || b; });
}

void CSG::subtractInplace(const std::vector<bool>& a, std::vector<bool>& b) {
  apply(a, b, [](bool a, bool b) {return b && (!a); });
}

void CSG::apply(const std::vector<bool>& a, std::vector<bool>& b, std::function<bool(bool, bool)> fun) {
  assert(a.size() == b.size());
  assert(a.size() % 8 == 0);
#pragma omp parallel for
  for (long long i = 0; i < a.size(); i+=8) {
    for (int sub = 0; sub < 8; sub++) { //std::vector<bool> is a special flower in terms of parallelism
      b[i + sub] = fun(a[i + sub], b[i + sub]);
    }
  }
}

void CSG::apply(std::vector<double>& a, std::function<double(int, int, int)> fun, const glm::ivec3& resolution) {
#pragma omp parallel for
  for (int x = 0; x < resolution.x; x++) {
    for (int y = 0; y < resolution.y; y++) {
      for (int z = 0; z < resolution.z; z++) {
        int address = z+y*resolution.z+resolution.z*resolution.y*x;
        a[address] = fun(x, y, z);
      }
    }
  }
}

void CSG::applySingleCore(std::vector<double>& a, std::function<double(int, int, int)> fun, const glm::ivec3& resolution) {
  for (int x = 0; x < resolution.x; x++) {
    for (int y = 0; y < resolution.y; y++) {
      for (int z = 0; z < resolution.z; z++) {
        int address = z + y * resolution.z + resolution.z * resolution.y * x;
        a[address] = fun(x, y, z);
      }
    }
  }
}
