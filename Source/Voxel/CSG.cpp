#include "CSG.h"

#include <cassert>

void CSG::andInplace(const std::vector<bool>& a, std::vector<bool>& b) {
  apply(a, b, [](bool a, bool b) {return a && b; });
}

void CSG::orInplace(const std::vector<bool>& a, std::vector<bool>& b) {
  apply(a, b, [](bool a, bool b) {return a || b; });
}

void CSG::apply(const std::vector<bool>& a, std::vector<bool>& b, std::function<bool(bool, bool)> fun) {
  assert(a.size() == b.size());
  assert(a.size() % 8 == 0);
#pragma omp parallel for
  for (long long i = 0; i < a.size(); i+=8) {
    for (int sub = 0; sub < 8; sub++) { //std::vector<bool> is a special flower in terms of parallelism
      b[i + sub] = fun(b[i + sub], a[i + sub]);
    }
  }
}