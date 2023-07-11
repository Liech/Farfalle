#include "LineSoup.h"

#include <set>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Surface_sweep_2_algorithms.h>
#include <CGAL/intersections.h>
#include <CGAL/Segment_2.h>
#include <CGAL/number_utils.h>



#include "Model.h"


typedef CGAL::Exact_predicates_exact_constructions_kernel       Kernel;
typedef Kernel::Point_2                                         Point_2;
typedef CGAL::Arr_segment_traits_2<Kernel>                      Traits_2;
typedef Kernel::Segment_2                                       Segment_2;

class LineSoup_PIMPL {
public:
  std::vector< Segment_2> soup;
};

LineSoup::LineSoup(const Model& model) : target(model){
  p = std::make_shared<LineSoup_PIMPL>();
  size_t faces = model.getNumberFaces();
  p->soup.reserve(faces * 3 / 2);
  std::set<std::pair<size_t, size_t>> used;

  for (size_t faceID = 0; faceID < faces; faceID++) {
    auto vi = model.getFaceIndices(faceID); //vertex indices
    glm::dvec3 p1 = model.getVertexPosition(vi[0]);
    glm::dvec3 p2 = model.getVertexPosition(vi[1]);
    glm::dvec3 p3 = model.getVertexPosition(vi[2]);

    if (!used.contains(std::make_pair(vi[0], vi[1]))) {
      addToSoup(std::make_pair(p1, p2));
      used.insert(std::make_pair(vi[0], vi[1]));
      used.insert(std::make_pair(vi[1], vi[0]));
    }
    if (!used.contains(std::make_pair(vi[0], vi[1]))) {
      addToSoup(std::make_pair(p2, p3));
      used.insert(std::make_pair(vi[1], vi[2]));
      used.insert(std::make_pair(vi[2], vi[1]));
    }
    if (!used.contains(std::make_pair(vi[0], vi[1]))) {
      addToSoup(std::make_pair(p3, p1));
      used.insert(std::make_pair(vi[2], vi[0]));
      used.insert(std::make_pair(vi[0], vi[2]));
    }
  }
}

std::vector<std::pair<glm::dvec2, glm::dvec2>> LineSoup::segmentCut(glm::dvec2 start, glm::dvec2 end) {
  Segment_2 seg(Point_2(start.x, start.y), Point_2(end.x, end.y));
  std::vector<glm::dvec2> intersections;

  //todo: use something else than poor mans brute force
  for (auto& line : p->soup) {
    auto x = CGAL::intersection(line, seg);
    if (x.has_value() && x.value().type() == typeid(Point_2)) {
      Point_2 p = boost::get<Point_2>(x.value());
      double xcoord = CGAL::to_double(p.x());
      double ycoord = CGAL::to_double(p.y());
      glm::dvec2 intersectionPoint = glm::dvec2(xcoord, ycoord);
      intersections.push_back(intersectionPoint);
    }
  }

  //sort by distance
  std::sort(intersections.begin(), intersections.end(), [start](const glm::dvec2& a, const glm::dvec2& b) -> bool
    {
      return glm::distance(start, a) > glm::distance(start, b);
    });

  std::vector<std::pair<glm::dvec2, glm::dvec2>> result;

  result.push_back(std::make_pair(start, intersections[0]));

  for (int i = 1; i < intersections.size(); i++) {
    result.push_back(std::make_pair(intersections[i - 1], intersections[i]));
  }

  return result;
}

void LineSoup::save(const std::string& filename) {

}


void LineSoup::addToSoup(const std::pair<glm::dvec3, glm::dvec3>& pair) {
  glm::dvec2 a = target.world2UV(pair.first);
  glm::dvec2 b = target.world2UV(pair.second);
  p->soup.push_back(Segment_2(Point_2(a.x,a.y), Point_2(b.x,b.y)));
}