#include "Macaronifyer.h"

#include <iostream>

#include "Tools/LineSimplification.h"
#include "Tools/MeshSurfaceOffsetting.h"
#include "SliceConfig.h"
#include "Model.h"

/*

Marching Triangles isoline distance finding:
one full subdivision -> prohibits 3 Zero Triangles
search&mark for edges enclosing desired value
pick edge -> find if triangles have other marked edges -> travel along
  visited edge?->loop finished
  no edge left -> just streak (degenerated case)
all marked edges visted? no? ->pick one of the remaining edges and repeat
*/
Macaronifyer::Macaronifyer(const SliceConfig& config) {

}

Macaronifyer::~Macaronifyer() {

}

std::vector<std::vector<glm::dvec3>> Macaronifyer::macaronify(Model& toNoodlize) const {
  std::vector<std::vector<glm::dvec3>> result;

  result = toNoodlize.getBorder();
  for (int i = 0; i < result.size(); i++) {
    result[i].push_back(result[i][0]);
  }

  auto soup = toNoodlize.toSoup();
  MeshSurfaceOffsetting offseting(soup.first, soup.second);
  offseting.saveInternalMesh("dbg/offsetting.stl");
  auto offset = offseting.offset(0.4);

  for (auto& loop : offset.closed) {
    result.push_back(loop);
    result[result.size() - 1].push_back(loop[0]);
  }
  for (auto& streak : offset.open) {
    result.push_back(streak);
  }

  return result;
}
