#include "Mesh2D.h"

#include <set>

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
#include <CGAL/Line_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/IO/STL.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Boolean_set_operations_2.h>


typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Exact_predicates_inexact_constructions_kernel       Kernel;
typedef Kernel::Point_3                                           Point3;
typedef Kernel::Ray_2                                             Ray2;
typedef Kernel::Line_2                                            Line_2;
typedef Kernel::Point_2                                           Point_2;
typedef Kernel::Segment_2                                         Segment2;
typedef CGAL::Polygon_with_holes_2<Kernel>                Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2>                   Pwh_list_2;
typedef CGAL::Polygon_2<K>                                Polygon_2;
namespace PS = CGAL::Polyline_simplification_2;
typedef PS::Stop_below_count_ratio_threshold Stop;
typedef PS::Squared_distance_cost            Cost;
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_2<K> Vb;
typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> CDT;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
typedef CDT::Vertex_handle Vertex_handle;
typedef CDT::Point Point;

class Mesh2DPIMPL {
public:
  std::vector<Polygon_with_holes_2> loopSoup2Polys(const std::vector<std::vector<Point_2>>&);

 std::vector<Polygon_with_holes_2> poly;
};

Mesh2D::~Mesh2D() {

}

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
        inside = inside && CGAL::oriented_side(x, p);

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
  auto decomposer =CGAL::Polygon_vertical_decomposition_2< Kernel>(); // p2.w.holes -> p2 does not exist :(?
  for (auto x : p->poly)
    decomposer(x, std::back_inserter(polys));
  std::vector<std::shared_ptr<Mesh2D>> result;
  for (auto x : polys) {
    std::shared_ptr<Mesh2D> m = std::make_shared<Mesh2D>();
    m->p->poly = { Polygon_with_holes_2(x, empty.begin(),empty.end()) };
    result.push_back(m);
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

//returns sorted collision points along the given line
std::vector<glm::dvec2> Mesh2D::intersectLine(const glm::dvec2& start, const glm::dvec2& end) const{
  std::vector<glm::dvec2> result;
  Line_2 l(Point_2(start.x, start.y), Point_2(end.x, end.y));

  //Intersection
  auto intersect = [&result, l](const Polygon_2& poly) {
    auto& vertecies = poly.vertices();

    auto insertEntry = [&result](const Point_2& p) {
      glm::dvec2 glmP = glm::dvec2(p.x(), p.y());
      result.push_back(glmP);
      };

    auto lineIntersect = [&result, insertEntry](const Line_2& line, const Segment2& seg) {
      auto subResult = CGAL::intersection(line, seg);
      if (subResult.has_value()) {
        if (subResult.value().type() == typeid(Point_2)) {
          insertEntry(boost::get<Point_2>(subResult.value()));
        }
        else if (subResult.value().type() == typeid(Segment2)) {
          insertEntry(boost::get<Segment2>(subResult.value()).start());
          insertEntry(boost::get<Segment2>(subResult.value()).end());
        }
      }
    };

    for (int i = 1; i < vertecies.size(); i++) {
      Segment2 seg = Segment2(vertecies[i - 1], vertecies[i]);
      lineIntersect(l, seg);
    }
    Segment2 wrapAround = Segment2(vertecies[vertecies.size() - 1], vertecies[0]);
    lineIntersect(l, wrapAround);
  };
  
  //Loop through polygons
  for (auto& poly : p->poly) {
    intersect(poly.outer_boundary());
    for (auto& hole : poly.holes())
      intersect(hole);
  }

  //sort by along line
  glm::dvec2 sortDirection = glm::normalize(start - end);
  std::sort(result.begin(), result.end(),   
    [sortDirection](const glm::dvec2& a, const glm::dvec2& b) {
      //Non Normalized Dot Product should result in an good value for directional sort
      return glm::dot(sortDirection,a) > glm::dot(sortDirection,b); }
  );

  return result;
}

glm::dvec2 Mesh2D::raycast(const glm::dvec2& start, const glm::dvec2& dir, int index, bool& doesIntersect) {
  doesIntersect = true;
  auto bound = p->poly[index].outer_boundary();
  Ray2 l = Ray2(Point_2(start.x, start.y), Point_2(start.x + dir.x, start.y + dir.y));
  double bestDistance = std::numeric_limits<double>::max();
  glm::dvec2 bestIntersection;
  glm::dvec2 bestNormal;
  for (int i = 1; i < bound.vertices().size(); i++) {
    Segment2 s(bound.vertex(i - 1), bound.vertex(i));
    auto insertEntry = [&bestDistance, &bestIntersection, start,s](const Point_2& p) {
      auto current = glm::dvec2(p.x(), p.y());
      double currentDistance = glm::distance(current, start);
      if (currentDistance < bestDistance) {
        bestDistance = currentDistance;
        bestIntersection = current;
      }
    };
    auto subResult = CGAL::intersection(l, s);
    if (subResult.has_value()) {
      if (subResult.value().type() == typeid(Point_2)) {
        insertEntry(boost::get<Point_2>(subResult.value()));
      }
      else if (subResult.value().type() == typeid(Segment2)) {
        insertEntry(boost::get<Segment2>(subResult.value()).start());
        insertEntry(boost::get<Segment2>(subResult.value()).end());
      }
    }
  }
  if (bestDistance == std::numeric_limits<double>::max())
    doesIntersect = false;
  return bestIntersection;
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
  bool doesIntersect = false;
  currentPosition = raycast(currentPosition, glm::dvec2(0, 1), index, doesIntersect);
  result.push_back(currentPosition);
  if (!doesIntersect) return {};

  double directionSign = -1;
  while (doesIntersect) {
    currentPosition = raycast(currentPosition, glm::dvec2(0, directionSign), index, doesIntersect);
    result.push_back(currentPosition);
    currentPosition += glm::dvec2(distance, 0);
    currentPosition = raycast(currentPosition, glm::dvec2(0, directionSign), index, doesIntersect);
    directionSign *= -1;
    if (doesIntersect)
      result.push_back(currentPosition);
  }

  return result;
}
//
////point of poly that is farthest placed into direction of "north"
////e.g. if north is -Xaxis it would be a point with: point.x < AllOther.x 
//glm::dvec2 Mesh2D::getPolePoint(const glm::dvec2& north) const {
//  double     bestMetric = 0;
//  glm::dvec2 bestPoint = glm::dvec2(0, 0);<
//
//  auto evaluate = [north](const glm::dvec2& point) {
//    glm::dvec2 projectedPoint;// = closestpointoninfiniteline
//    //project on line
//    //get signed distancce as metric
//    //compare and maybe replace best
//  };
//
//  for (auto& poly : p->poly) {
//    for(auto point : poly.outer_boundary().vertices())
//      evaluate(glm::dvec2(point.x(), point.y()));
//    for(auto hole : poly.holes())
//      for(auto point : hole.vertices())
//        evaluate(glm::dvec2(point.x(), point.y()));
//  }
//
//  return bestPoint;
//}

std::pair<std::vector<glm::dvec2>, std::vector<int>> Mesh2D::getTriangulation(Model* representaion3D) {
  CDT cdt;


  std::set<std::array<double,2>> allPoints;
  if (representaion3D != nullptr) {
    size_t amountFaces = representaion3D->getNumberFaces();
    for (size_t face = 0; face < amountFaces; face++) {
      for (auto index : representaion3D->getFaceIndices(face)) {
        auto pos = representaion3D->world2UV(representaion3D->getVertexPosition(index));
        auto arrayPos = std::array<double, 2>{pos.x, pos.y};
        if (allPoints.count(arrayPos) == 0) {
          allPoints.insert(arrayPos);
          cdt.push_back(Point_2(pos.x, pos.y));
        }
      }
    }
  }
  
  std::list<Point> list_of_seeds;

  auto addSeed = [&list_of_seeds](glm::dvec2 a, glm::dvec2 b) {
    glm::dvec2 dir = glm::normalize(b - a);
    glm::dvec2 outerDir = glm::dvec2(-dir.y, dir.x) * 1e-8;
    glm::dvec2 seedPos = glm::dvec2(a + b) / 2.0 + outerDir;
    list_of_seeds.push_back(Point(seedPos.x, seedPos.y));
  };

  for (auto& poly : p->poly) {
    int outerSize = poly.outer_boundary().size();
    for (int i = 0; i < outerSize; i++) {
      int current = i;
      int previous = (i == 0) ? (outerSize-1) : (i - 1);
      Point_2 currentP = poly.outer_boundary().vertex(current);
      Point_2 previousP= poly.outer_boundary().vertex(previous);
      cdt.insert_constraint(currentP, previousP);
      allPoints.insert(std::array<double, 2>{currentP.x(), currentP.y()});
      allPoints.insert(std::array<double, 2>{previousP.x(), previousP.y()});
    }
    for (auto& hole : poly.holes()) {
      int holeSize = hole.size();
      for (int i = 0; i < holeSize; i++) {
        int current = i;
        int previous = (i == 0) ? (holeSize - 1) : (i - 1);
        Point_2 currentP  = hole.vertex(current);
        Point_2 previousP = hole.vertex(previous);
        cdt.insert_constraint(currentP, previousP);
        allPoints.insert(std::array<double, 2>{currentP.x(), currentP.y()});
        allPoints.insert(std::array<double, 2>{previousP.x(), previousP.y()});
      }

      glm::dvec2 hsecond = glm::dvec2(hole.vertex(0).x(), hole.vertex(0).y());
      glm::dvec2 hfirst = glm::dvec2(hole.vertex(1).x(), hole.vertex(1).y());
      addSeed(hfirst, hsecond);
    }
  
    glm::dvec2 second  = glm::dvec2(poly.outer_boundary().vertex(0).x(), poly.outer_boundary().vertex(0).y());
    glm::dvec2 first = glm::dvec2(poly.outer_boundary().vertex(1).x(), poly.outer_boundary().vertex(1).y());
    //addSeed(first, second);
  }


  CGAL::refine_Delaunay_mesh_2(cdt, list_of_seeds.begin(), list_of_seeds.end(), Criteria());

  typedef CGAL::internal::CC_iterator<CGAL::Compact_container<CGAL::Triangulation_vertex_base_2<CGAL::Exact_predicates_inexact_constructions_kernel, CGAL::Triangulation_ds_vertex_base_2<Tds>>>, false> vertexPointer;
  std::map< vertexPointer, int> indexMap;
  
  std::pair<std::vector<glm::dvec2>, std::vector<int>> result = std::make_pair(std::vector<glm::dvec2>{}, std::vector<int>{});
  std::vector<glm::dvec2>& resultPoints = result.first;
  std::vector<int>&        resultIndices = result.second;

  int counter = 0;
  for (CDT::Finite_faces_iterator it = cdt.finite_faces_begin(); it != cdt.finite_faces_end(); ++it) {
    if (!it->is_in_domain())
      continue;    
       
    for (int i = 0; i < 3; i++) {
      vertexPointer v = it->vertex(i);
      if (indexMap.count(v) == 0) {
        indexMap[v] = counter;
        auto point = cdt.point(v);
        resultPoints.push_back(glm::dvec2(point.x(), point.y()));
        counter++;
      }
      resultIndices.push_back(indexMap[v]);
    }
  }
  return result;
}
