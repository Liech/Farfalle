#include "Mesh2D.h"

#include "Model.h"
#include "Tools/glm2svg.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/STL.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polyline_simplification_2/simplify.h>
#include <CGAL/Polygon_vertical_decomposition_2.h>
#include <CGAL/Ray_2.h>


typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Exact_predicates_inexact_constructions_kernel       Kernel;
typedef Kernel::Point_3                                           Point3;
typedef Kernel::Ray_2                                           Ray2;
typedef Kernel::Point_2                                   Point_2;
typedef CGAL::Polygon_with_holes_2<Kernel>                Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2>                   Pwh_list_2;
typedef CGAL::Polygon_2<K>                                Polygon_2;
namespace PS = CGAL::Polyline_simplification_2;
typedef PS::Stop_below_count_ratio_threshold Stop;
typedef PS::Squared_distance_cost            Cost;

class Mesh2DPIMPL {
public:
  std::vector<Polygon_with_holes_2> loopSoup2Polys(const std::vector<std::vector<Point_2>>&);

 std::vector<Polygon_with_holes_2> poly;
};

Mesh2D::Mesh2D(const std::vector<std::vector<glm::dvec3>>& loops) {
  std::vector<std::vector<glm::dvec2>> loops2d;
  loops2d.resize(loops.size());
  for (size_t i = 0; i < loops.size();i++) {
    loops2d[i].resize(loops[i].size());
    for (size_t j = 0; j < loops[i].size(); j++) {
      loops2d[i][j] = loops[i][j];
    }
  }
  init(loops2d);
}

Mesh2D::Mesh2D(Model& modl, const std::vector<std::vector<glm::dvec3>>& loops) {
  modl.generateUVMap();

  std::vector<std::vector<glm::dvec2>> loops2d;
  loops2d.resize(loops.size());
  for (size_t i = 0; i < loops.size(); i++) {
    loops2d[i].resize(loops[i].size());
    for (size_t j = 0; j < loops[i].size(); j++) {
      loops2d[i][j] = modl.world2UV(loops[i][j]);
    }
  }

  init(loops2d);
}

Mesh2D::Mesh2D() {
  init({});
}

void Mesh2D::savePoly(const std::string& filename) const{
  std::vector<std::vector<glm::dvec2>> streaks;
  streaks.reserve(p->poly.size());

  std::vector<std::string> colors;
  for (auto& x : p->poly) {
    std::vector<glm::dvec2> streak;

    colors.push_back("black");
    for (auto y : x.outer_boundary())
      streak.push_back(glm::dvec2(y.x(), y.y()));
    streak.push_back(glm::dvec2(x.outer_boundary()[0].x(), x.outer_boundary()[0].y()));
    streaks.push_back(streak);

    for (auto& x : x.holes()) {
      colors.push_back("red");
      std::vector<glm::dvec2> streak;
      for (auto y : x)
        streak.push_back(glm::dvec2(y.x(), y.y()));
      streak.push_back(glm::dvec2(x[0].x(), x[0].y()));
      streaks.push_back(streak);
    }

  }
  Tools::glm2svg().save(filename, streaks, colors);
}

void Mesh2D::init(const std::vector<std::vector<glm::dvec2>>& loops) {
  p = std::make_shared<Mesh2DPIMPL>();
  //https://doc.cgal.org/latest/Mesh_2/index.html

  std::vector<std::vector<Point_2>> converted;
  converted.resize(loops.size());
  for (size_t i = 0; i < loops.size(); i++)
    for (size_t j = 0; j < loops[i].size(); j++)
      converted[i].push_back(Point_2(loops[i][j].x, loops[i][j].y));
  p->poly = p->loopSoup2Polys(converted);
}

std::shared_ptr<Mesh2D> Mesh2D::difference(const Mesh2D& other) {
  std::shared_ptr<Mesh2D> result = std::make_shared<Mesh2D>();
  //savePoly("dbg/differenceMe.svg");
  //other.savePoly("dbg/differenceOther.svg");

  for (size_t a = 0; a < p->poly.size(); a++) {
    //https://doc.cgal.org/latest/Boolean_set_operations_2/index.html


    for (size_t b = 0; b < other.p->poly.size(); b++) {
      CGAL::difference(p->poly[a], other.p->poly[b], std::back_inserter(result->p->poly));
    }
  }

  return result;
}

std::vector<Polygon_with_holes_2> Mesh2DPIMPL::loopSoup2Polys(const std::vector<std::vector<Point_2>>& loops) {
  Cost cost;

  //identify loops by loop orientation
  std::vector<int> holeIndex;
  std::vector<int> polyIndex;
  for (size_t i = 0; i < loops.size(); i++) {
    auto& loop = loops[i];
    bool isHole = CGAL::Orientation::NEGATIVE == CGAL::orientation_2(loop.begin(), loop.end());

    std::vector<Point_2> p;
    for (auto& x : loop)
      p.push_back(Point_2(x.x(), x.y()));
    isHole = Polygon_2(p.begin(), p.end()).is_clockwise_oriented();

    if (isHole)
      holeIndex.push_back(i);
    else {
      //if (!Polygon_2(p.begin(), p.end()).is_counterclockwise_oriented())
      //  throw std::runtime_error(":(");
      polyIndex.push_back(i);
    }
  }
  
  //prep outer polys
  //to find the smallest loop containing the hole -> save radius
  std::vector<Polygon_2> outer;
  std::vector<double> area;
  for (size_t i = 0;i < polyIndex.size(); i++) {
    auto& loop = loops[polyIndex[i]];
    Polygon_2 poly = Polygon_2();
    for (auto& x : loop)
      poly.push_back(x);  
    poly = PS::simplify(poly, cost, Stop(0.5));


    outer.push_back(poly);
    area.push_back(poly.area());
  }

  //assign holes to polys
  std::vector<std::vector<Polygon_2>> assignedHoles;
  assignedHoles.resize(outer.size());
  for (size_t i = 0; i < holeIndex.size(); i++) {
    auto& hole = loops[holeIndex[i]];
    double bestArea = std::numeric_limits<double>::max();
    size_t bestPoly = 0;

    for (size_t poly = 0; poly < outer.size(); poly++) {
      auto& p = outer[poly];
      bool inside = CGAL::oriented_side(hole[0], p);
      for(auto& x: hole) 
        inside &= CGAL::oriented_side(x, p);

      bool areaBetter = bestArea > area[bestPoly];
      if (inside && areaBetter) {
        bestPoly = poly;
        bestArea = area[bestPoly];
      }
    }

    Polygon_2 h = Polygon_2();
    for (auto& x : hole)      
      h.push_back(x);
    h = PS::simplify(h, cost, Stop(0.5));

    assignedHoles[bestPoly].push_back(h);
  }

  //build result
  std::vector<Polygon_with_holes_2> result;
  for (size_t i = 0; i < outer.size(); i++) {
    result.push_back(Polygon_with_holes_2(outer[i], assignedHoles[i].begin(), assignedHoles[i].end()));
  }

  return result;
}

std::vector<std::shared_ptr<Mesh2D>> Mesh2D::decompose() {
  //https://doc.cgal.org/latest/Minkowski_sum_2/classPolygonWithHolesConvexDecomposition__2.html
  //PolygonWithHolesConvexDecomposition_2
  std::vector<Polygon_2> polys;
  std::vector<Polygon_2> empty;
  auto decomposer =CGAL::Polygon_vertical_decomposition_2< Kernel>();
  for (auto x : p->poly)
    decomposer(x, std::back_inserter(polys));
  std::vector<std::shared_ptr<Mesh2D>> result;
  for (auto x : polys) {
    std::shared_ptr<Mesh2D> m = std::make_shared<Mesh2D>();
    m->p->poly = { Polygon_with_holes_2(x, empty.begin(),empty.end()) };
  }
  return result;
}

std::vector<std::vector<glm::dvec2>> Mesh2D::fill(double distance) {
  std::vector<std::vector<glm::dvec2>> result;

  for (int i = 0; i < p->poly.size();i++) {
    if (p->poly[i].has_holes())
      throw std::runtime_error("Does not work with holes!");
    auto sub = fill(distance, i);
    if (sub.size() != 0)
      result.push_back(sub);
  }
  return result;
}

std::vector<glm::dvec2> Mesh2D::fill(double distance, int index) {
  std::vector<glm::dvec2> result;
  auto get = [this, index](int vertex) {
    auto result = p->poly[index].outer_boundary().vertex(vertex);
    return glm::dvec2(result.x(), result.y());
    };

  //find leftmost
  glm::dvec2 start = get(0);
  int numberVertex = p->poly[index].outer_boundary().vertices().size();
  for (int i = 0; i < numberVertex; i++) {
    auto glmVertex = get(i);
    if (start.x > glmVertex.x)
      start = glmVertex;
  }

  glm::dvec2 currentPosition = start;
  currentPosition.x += distance;
  auto bound = p->poly[index].outer_boundary();
  typedef CGAL::Polygon_set_2<Kernel, std::vector<Kernel::Point_2>> Polygon_set_2;
  Polygon_set_2 ps;
  ps.insert(bound);
  auto raycast = [ps, bound](const glm::dvec2& start, const glm::dvec2& dir) {
    //todo: this poly line intersection the cgal way does looks really weird. I guess im missing something here, theres got to be a better way
    Ray2 l = Ray2(Point_2(start.x, start.y), Point_2(start.x + dir.x, start.y + dir.y));
    std::vector<Polygon_with_holes_2> list;
    std::vector<Point_2> rawRayPoly{};
    Polygon_2 ray(rawRayPoly.begin(), rawRayPoly.end());
    auto sub = CGAL::intersection(ray, bound, std::back_inserter(list));

    };


  return {};
}
