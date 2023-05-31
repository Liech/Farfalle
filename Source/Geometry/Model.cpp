#include "Model.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/triangulate_faces.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <CGAL/boost/graph/helpers.h>

#include <iostream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3                                     Point;
typedef CGAL::Surface_mesh<Point>                           Surface_mesh;
namespace PMP = CGAL::Polygon_mesh_processing;

Model::Model() {
  std::string filename = "C:\\Users\\nicol\\Downloads\\_3DBenchy_-_The_jolly_3D_printing_torture-test_by_CreativeTools_se_763622\\files\\3DBenchy.stl";
  //std::vector<CGAL::cpp11::array<double, 3> > points;
  //std::vector<CGAL::cpp11::array<int, 3> > triangles;
  //CGAL::IO::read_STL(filename, points, triangles);
  //std::cout << "Loading: " << points.size() << " - " << triangles.size() << std::endl;

  Surface_mesh mesh;
  if (!PMP::IO::read_polygon_mesh(filename, mesh))
  {
    std::cerr << "Invalid input." << std::endl;
    return;
  }

  std::cout << "Loaded: " << mesh.num_vertices() << " - " << mesh.num_faces() << std::endl;
}