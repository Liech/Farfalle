#include "Model.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/boost/graph/helpers.h>
#include <CGAL/Polygon_mesh_slicer.h>

#include <iostream>

namespace PMP = CGAL::Polygon_mesh_processing;

typedef CGAL::Exact_predicates_inexact_constructions_kernel       Kernel;
typedef Kernel::Point_3                                           Point;
typedef CGAL::Surface_mesh<Point>                                 Surface_mesh;
typedef std::vector<Kernel::Point_3>                              Polyline_type;
typedef std::list<Polyline_type>                                  Polylines;
typedef CGAL::AABB_halfedge_graph_segment_primitive<Surface_mesh> HGSP;
typedef CGAL::AABB_traits<Kernel, HGSP>                           AABB_traits;
typedef CGAL::AABB_tree<AABB_traits>                              AABB_tree;


class ModelPimpl {
public:
  Surface_mesh mesh;
  AABB_tree    tree;
};

Model::Model(const std::string& filename) {
  std::cout << "Loading Mesh: " << filename << std::endl;
  p = std::make_shared<ModelPimpl>();

  Surface_mesh mesh;
  if (!PMP::IO::read_polygon_mesh(filename, mesh))
  {
    std::cerr << "Invalid input." << std::endl;
    throw std::runtime_error("Invalid Input");
    return;
  }
  p->mesh = mesh;
  std::cout << "Loaded: " << mesh.num_vertices() << " - " << mesh.num_faces() << std::endl;
  glm::dvec3 min = glm::dvec3(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
  glm::dvec3 max = glm::dvec3(std::numeric_limits<double>::min(), std::numeric_limits<double>::min(), std::numeric_limits<double>::min());
  for (auto x : p->mesh.points()) {
    if (x.x() < min[0]) min[0] = x.x();
    if (x.y() < min[1]) min[1] = x.y();
    if (x.z() < min[2]) min[2] = x.z();
    if (x.x() > max[0]) max[0] = x.x();
    if (x.y() > max[1]) max[1] = x.y();
    if (x.z() > max[2]) max[2] = x.z();
  }
  std::cout << "MIN: " << min[0] << "/" << min[1] << "/" << min[2] << std::endl;
  std::cout << "MAX: " << max[0] << "/" << max[1] << "/" << max[2] << std::endl;

  std::cout << "Build Tree..." << std::endl;
  p->tree = AABB_tree(edges(p->mesh).first, edges(p->mesh).second, p->mesh);
}

std::vector<std::vector<glm::dvec3>> Model::slice(const glm::dvec3& normal, double z) {
  std::cout << "Slice Mesh..."<< std::endl;
  Polylines polylines;

  CGAL::Polygon_mesh_slicer<Surface_mesh, Kernel> slicer_aabb(p->mesh, p->tree);
  slicer_aabb(Kernel::Plane_3(normal[0], normal[1], normal[2], -z), std::back_inserter(polylines));
  
  std::vector<std::vector<glm::dvec3>> result;
  for (auto line : polylines) {
    std::vector<glm::dvec3> sub;
    for (auto x : line)
      sub.push_back(glm::dvec3(x.x(), x.y(), x.z()));
    result.push_back(sub);
  }
  return result;
}