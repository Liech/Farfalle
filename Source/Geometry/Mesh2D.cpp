#include "Mesh2D.h"

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
  p = std::make_shared<Mesh2DPIMPL>();
  //https://doc.cgal.org/latest/Mesh_2/index.html
  Vertex_handle va = p->cdt.insert(Point(2, 0));
  Vertex_handle vb = p->cdt.insert(Point(0, 2));
  Vertex_handle vc = p->cdt.insert(Point(-2, 0));
  Vertex_handle vd = p->cdt.insert(Point(0, -2));
  p->cdt.insert_constraint(va, vb);
  p->cdt.insert_constraint(vb, vc);
  p->cdt.insert_constraint(vc, vd);
  p->cdt.insert_constraint(vd, va);
  va = p->cdt.insert(Point(3, 3));
  vb = p->cdt.insert(Point(-3, 3));
  vc = p->cdt.insert(Point(-3, -3));
  vd = p->cdt.insert(Point(3, 0 - 3));
  p->cdt.insert_constraint(va, vb);
  p->cdt.insert_constraint(vb, vc);
  p->cdt.insert_constraint(vc, vd);
  p->cdt.insert_constraint(vd, va);
  std::list<Point> list_of_seeds;
  list_of_seeds.push_back(Point(0, 0));
  std::cout << "Number of vertices: " << p->cdt.number_of_vertices() << std::endl;
  std::cout << "Meshing the domain..." << std::endl;
  CGAL::refine_Delaunay_mesh_2(p->cdt, list_of_seeds.begin(), list_of_seeds.end(),
    Criteria());
  std::cout << "Number of vertices: " << p->cdt.number_of_vertices() << std::endl;
  std::cout << "Number of finite faces: " << p->cdt.number_of_faces() << std::endl;
  int mesh_faces_counter = 0;
  for (CDT::Finite_faces_iterator fit = p->cdt.finite_faces_begin();
    fit != p->cdt.finite_faces_end(); ++fit)
  {
    if (fit->is_in_domain()) ++mesh_faces_counter;
  }
  std::cout << "Number of faces in the mesh domain: " << mesh_faces_counter << std::endl;

  save("slice.stl");
}

void Mesh2D::save(const std::string& filename) {
  std::vector<Point3> points;
  std::vector<std::vector<std::size_t> > polygons;
  points.reserve(p->cdt.number_of_vertices());

  //for (Point x : p->cdt.points()){
  //  points.push_back(Point3(x.x(),x.y(),0));
  //}
  size_t counter = 0;
  for (auto it = p->cdt.faces_begin(); it != p->cdt.faces_end(); ++it) {
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