#include "Primitives.h"

#include "CGALDefs.h"

#define M_PI 3.14159265358979323846264338327950288   /* pi */

#include "Model.h"
#include "ModelImplementation.h"
  
std::unique_ptr<Model> Primitives::sphere(const glm::dvec3& position, double radius, int xyAmount, int zAmount) {
  //https://stackoverflow.com/questions/33197841/create-parameterized-3d-primitives-with-cgal
  
  std::vector<Point> pts;
  pts.reserve(zAmount * xyAmount);

  for (int xy = 0; xy < xyAmount; xy++) {
    for (int z = 0; z < zAmount; z++) {
      double zPos  = M_PI * (double)z  / (double)zAmount ;
      double xyPos = 2*M_PI * (double)xy / (double)xyAmount;
      glm::dvec3 p;
      p[0] = radius * std::cos(xyPos) * std::sin(zPos);
      p[1] = radius * std::sin(xyPos) * std::sin(zPos);
      p[2] = radius * std::cos(zPos);
      pts.push_back(Point(p[0],p[1],p[2]));
    }
  }

  std::cout << "convex hull.." << std::endl;

  std::unique_ptr<ModelImplementation> impl = std::make_unique<ModelImplementation>();
  CGAL::convex_hull_3(pts.begin(), pts.end(), impl->mesh);
  auto result = std::make_unique<Model>(std::move(impl));
  return std::move(result);
}