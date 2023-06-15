#include "Mesh2D.h"

#include "Model.h"
#include "Tools/glm2svg.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
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
typedef CGAL::Triangulation_vertex_base_2<K> Vb;
typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> CDT;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
typedef CDT::Vertex_handle Vertex_handle;
typedef CDT::Point Point;
typedef CGAL::Exact_predicates_inexact_constructions_kernel       Kernel;
typedef Kernel::Point_3                                           Point3;
typedef Kernel::Point_2                                   Point_2;
typedef CGAL::Polygon_with_holes_2<Kernel>                Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2>                   Pwh_list_2;
typedef CGAL::Polygon_2<K>                                Polygon_2;

class Mesh2DPIMPL {
public:
  std::vector<std::shared_ptr<Polygon_with_holes_2>> loopSoup2Polys(const std::vector<std::vector<Point_2>>&);


  CDT                                                cdt;
  std::vector<std::shared_ptr<Polygon_with_holes_2>> poly;
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
      loops2d[i][j] = modl.getUV(loops[i][j]);
    }
  }

  init(loops2d);
}

void Mesh2D::saveCDT(const std::string& filename) {
  std::vector<Point3> points;
  std::vector<std::vector<std::size_t> > polygons;
  points.reserve(p->cdt.number_of_faces() * 3);

  size_t counter = 0;
  for (CDT::Finite_faces_iterator it = p->cdt.finite_faces_begin();
    it != p->cdt.finite_faces_end(); ++it)
  {
    
    if (it->is_in_domain())
      continue;

    auto v0 = p->cdt.point(it->vertex(0));
    auto v1 = p->cdt.point(it->vertex(1));
    auto v2 = p->cdt.point(it->vertex(2));
    points.push_back(Point3(v0.x(), v0.y(), 0));
    points.push_back(Point3(v1.x(), v1.y(), 0));
    points.push_back(Point3(v2.x(), v2.y(), 0));
    polygons.push_back({counter,counter+1,counter+2});
    counter += 3;
  }

  CGAL::IO::write_STL(filename, points, polygons);
}


void Mesh2D::savePoly(const std::string& filename) {
  std::vector<std::vector<glm::dvec2>> streaks;



  Tools::glm2svg().save(filename, streaks);
}

void Mesh2D::init(const std::vector<std::vector<glm::dvec2>>& loops) {
  p = std::make_shared<Mesh2DPIMPL>();
  //https://doc.cgal.org/latest/Mesh_2/index.html

  std::vector<std::vector<Point>> converted;
  converted.resize(loops.size());
  for (size_t i = 0; i < loops.size(); i++)
    for (size_t j = 0; j < loops[i].size(); j++)
      converted[i].push_back(Point_2(loops[i][j].x, loops[i][j].y));
  p->poly = p->loopSoup2Polys(converted);

  std::list<Point> list_of_seeds;
  for (auto& loop : loops)
  {
    glm::dvec2 vPrevious = glm::dvec2(loop[0].x, loop[0].y);
    Vertex_handle first = p->cdt.insert(Point(vPrevious.x, vPrevious.y));
    Vertex_handle previous = first;
    for (size_t i = 1; i < loop.size(); i++) {
      glm::dvec2    vCurrent = glm::dvec2(loop[i].x, loop[i].y);
      Vertex_handle current = p->cdt.insert(Point(vCurrent.x, vCurrent.y));
      p->cdt.insert_constraint(previous, current);
      if (i == 1) {
        glm::dvec2 dir = glm::normalize(vCurrent - vPrevious);
        glm::dvec2 outerDir = glm::dvec2(-dir.y, dir.x) * 1e-8;
        glm::dvec2 seedPos = (vCurrent + vPrevious) / 2.0 + outerDir;
        list_of_seeds.push_back(Point(seedPos.x, seedPos.y));
        //p->cdt.insert(Point(seedPos.x, seedPos.y));
      }
      previous = current;
      vPrevious = vCurrent;
    }
    p->cdt.insert_constraint(previous, first);
  }

  CGAL::refine_Delaunay_mesh_2(p->cdt, list_of_seeds.begin(), list_of_seeds.end(), Criteria());
  saveCDT("slice.stl");
}

std::shared_ptr<Mesh2D> Mesh2D::difference(const Mesh2D& other) {
  return nullptr;
}

std::vector<std::shared_ptr<Polygon_with_holes_2>> Mesh2DPIMPL::loopSoup2Polys(const std::vector<std::vector<Point_2>>& loops) {
  
  //identify loops by loop orientation
  std::vector<int> holeIndex;
  std::vector<int> polyIndex;
  for (size_t i = 0; i < loops.size(); i++) {
    auto& loop = loops[i];
    bool isHole = CGAL::Orientation::NEGATIVE == CGAL::orientation_2(loop.begin(), loop.end());
    if (isHole)
      holeIndex.push_back(i);
    else
      polyIndex.push_back(i);
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
      bool areaBetter = bestArea > area[bestPoly];
      if (inside && areaBetter) {
        bestArea = area[bestPoly];
        bestPoly = poly;
      }
    }

    Polygon_2 h = Polygon_2();
    for (auto& x : hole)
      h.push_back(x);
    assignedHoles[bestPoly].push_back(h);
  }

  //build result
  std::vector<std::shared_ptr<Polygon_with_holes_2>> result;
  for (size_t i = 0; i < outer.size(); i++) {
    std::shared_ptr<Polygon_with_holes_2> sub = std::make_shared<Polygon_with_holes_2>(outer[i],assignedHoles[i].begin(),assignedHoles[i].end());
    result.push_back(sub);
  }

  return result;
}