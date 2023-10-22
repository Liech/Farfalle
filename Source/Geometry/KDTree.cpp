#include "KDTree.h"

//https://doc.cgal.org/latest/Spatial_searching/Spatial_searching_2circular_query_8cpp-example.html
//https://doc.cgal.org/latest/Spatial_searching/Spatial_searching_2searching_with_point_with_info_8cpp-example.html

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Kd_tree.h>
#include <CGAL/algorithm.h>
#include <CGAL/Fuzzy_sphere.h>
#include <CGAL/Search_traits_3.h>
#include <boost/iterator/zip_iterator.hpp>

typedef CGAL::Simple_cartesian<double>                K;
typedef K::Point_3                                    Point_d;
typedef boost::tuple<Point_d, size_t>                 Point_and_Index;
typedef CGAL::Search_traits_3<K>                      Traits_base;
typedef CGAL::Search_traits_adapter<Point_and_Index,
  CGAL::Nth_of_tuple_property_map<0, Point_and_Index>,
  Traits_base>                                        Traits;

typedef CGAL::Fuzzy_sphere<Traits>                    Fuzzy_circle;
typedef CGAL::Kd_tree<Traits>                         Tree;

class KDTreePImpl {
public:
  Tree tree;
  std::vector<Point_and_Index> input;
};

KDTree::KDTree(const std::vector<glm::dvec3>& points) {
  p = std::make_unique<KDTreePImpl>();
  std::vector<Point_d> pointsCgal;
  pointsCgal.reserve(points.size());
  for (auto& x : points)
    pointsCgal.push_back(Point_d(x.x, x.y, x.z));
  std::vector<size_t> indices;
  indices.reserve(points.size());
  for (size_t i = 0; i < pointsCgal.size(); i++)
    indices.push_back(i);
  p->input = std::vector<Point_and_Index>(boost::make_zip_iterator(boost::make_tuple(pointsCgal.begin(), indices.begin())),
    boost::make_zip_iterator(boost::make_tuple(pointsCgal.end(), indices.end())));
  p->tree.insert(p->input.begin(),p->input.end());
}

KDTree::~KDTree() {

}

std::vector<size_t> KDTree::find(const glm::dvec3& position, double radius) {
  Point_d center(position.x,position.y,position.z);
  Fuzzy_circle default_range(center, radius);
  std::vector<Point_and_Index> queryResult;
  p->tree.search(std::back_inserter(queryResult), default_range);
  std::sort(queryResult.begin(), queryResult.end(), [position](const Point_and_Index& a, const Point_and_Index& b)
    {
      Point_d pA = boost::get<0>(a);
      Point_d pB = boost::get<0>(b);
      glm::dvec3 gA = glm::dvec3(pA.x(), pA.y(), pA.z());
      glm::dvec3 gB = glm::dvec3(pB.x(), pB.y(), pB.z());
      return glm::distance(gA, position) < glm::distance(gB, position);
    });

  std::vector<size_t> result;
  for (auto& x : queryResult) {
    auto i = boost::get<1>(x);
    Point_d point = boost::get<0>(p->input[i]);
    glm::dvec3 gPoint = glm::dvec3(point.x(), point.y(), point.z());
    if (glm::distance(position,gPoint) < radius)
      result.push_back(i);
  }
  return result;
}

void KDTree::remove(size_t d) {
  p->tree.remove(p->input[d]);
}
