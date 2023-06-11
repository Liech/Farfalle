#include "Mesh2D.h"

#include "Model.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/IO/STL.h>

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

class Mesh2DPIMPL {
public:

  CDT cdt;
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

void Mesh2D::save(const std::string& filename) {
  std::vector<Point3> points;
  std::vector<std::vector<std::size_t> > polygons;
  points.reserve(p->cdt.number_of_vertices());

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

void Mesh2D::init(const std::vector<std::vector<glm::dvec2>>& loops) {
  p = std::make_shared<Mesh2DPIMPL>();
  //https://doc.cgal.org/latest/Mesh_2/index.html
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
        glm::dvec2 outerDir = glm::dvec2(-dir.y, dir.x) * 1e-5;
        glm::dvec2 seedPos = (vCurrent + vPrevious) / 2.0 + outerDir;
        list_of_seeds.push_back(Point(seedPos.x, seedPos.y));
        p->cdt.insert(Point(seedPos.x, seedPos.y));
      }
      previous = current;
      vPrevious = vCurrent;
    }
    p->cdt.insert_constraint(previous, first);
  }

  CGAL::refine_Delaunay_mesh_2(p->cdt, list_of_seeds.begin(), list_of_seeds.end(), Criteria());
  save("slice.stl");
}