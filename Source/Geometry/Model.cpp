#include "Model.h"

#include <iostream>

#include "Primitives.h"
#include "CGALDefs.h"


class ModelPimpl {
public:
  Surface_mesh mesh;
  AABB_tree    tree;
};

Model::Model() {
  p = std::make_shared<ModelPimpl>();
}

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


  init();
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


std::shared_ptr<Model> Model::erode(double radius) const {
  //https://doc.cgal.org/latest/Minkowski_sum_3/index.html
  //https://github.com/CGAL/cgal/issues/6423
  std::cout << "Erosion..." << std::endl;
  std::shared_ptr<Model> result = std::make_shared<Model>();

  std::cout << "does_self_intersect: "<< CGAL::Polygon_mesh_processing::does_self_intersect(p->mesh) << std::endl;


  Nef_polyhedron nefSphere(*Primitives::sphere(glm::dvec3(0, 0, 0), radius));
  //Nef_polyhedron nefSphere(*Primitives::sphere(glm::dvec3(10, 10, 10), 10));
  Nef_polyhedron nefPoly  (p->mesh);
  std::cout << "Edges: " << nefPoly.number_of_edges() << std::endl;
  Nef_polyhedron eroded = CGAL::minkowski_sum_3(nefSphere, nefSphere);

  std::vector<Point>                     points;
  std::vector<std::vector<std::size_t> > polygons;

  //CGAL::convert_nef_polyhedron_to_polygon_soup(nefPoly, points, polygons, true);
  //CGAL::Polygon_mesh_processing::orient_polygon_soup(points,polygons);
  //Surface_mesh mesh;
  //CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, polygons, result->p->mesh);

  CGAL::convert_nef_polyhedron_to_polygon_mesh(eroded, result->p->mesh, true);
  //CGAL::convert_nef_polyhedron_to_polygon_mesh(nefSphere, result->p->mesh, true);

  result->init();
  return result;
}

glm::dvec3 Model::getMin() const {
  return min;
}

glm::dvec3 Model::getMax() const {
  return max;
}

void Model::repair() {

  std::cout << "Repairing..." << std::endl;
  assert(CGAL::is_valid_polygon_mesh(p->mesh));
  CGAL::Polygon_mesh_processing::remove_isolated_vertices(p->mesh);
  CGAL::Polygon_mesh_processing::duplicate_non_manifold_vertices(p->mesh);
  if (CGAL::Polygon_mesh_processing::does_self_intersect(p->mesh)) {
    std::cout << " * Remove Self Intersection..." << std::endl;
    if (!CGAL::Polygon_mesh_processing::experimental::remove_self_intersections(p->mesh, CGAL::parameters::preserve_genus(false))) {
      std::cout << "Self intersection repair failed!" << std::endl;
    } 
  }
  init();
}

void Model::init() {
  min = glm::dvec3(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
  max = glm::dvec3(std::numeric_limits<double>::min(), std::numeric_limits<double>::min(), std::numeric_limits<double>::min());
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