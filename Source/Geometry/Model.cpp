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
  p = std::make_shared<ModelPimpl>();

  Surface_mesh mesh;
  if (!PMP::IO::read_polygon_mesh(filename, mesh))
  {
    std::cerr << "Invalid input." << std::endl;
    throw std::runtime_error("Invalid Input");
    return;
  }
  p->mesh = mesh;
  init();
}

std::function<double(const glm::dvec3&)> implicitModelMakerFunction = [](const glm::dvec3& p) {return p.z; };
FT implicitFunctionModelMeshGen(const Point& p)
{
  return implicitModelMakerFunction(glm::dvec3(p.x(), p.y(), p.z()));
} 
 
Model::Model(std::function<double(const glm::dvec3&)>& func, const glm::dvec3& boundingSphereCenter, double boundingSphereRadius, double detail) {
  //https://doc.cgal.org/latest/Surface_mesher/index.html
  //https://doc.cgal.org/latest/Surface_mesh_parameterization/index.html
  assert(func(glm::dvec3(0, 0, 0)) <= 0);
  p = std::make_shared<ModelPimpl>();
  implicitModelMakerFunction = func;
  Tr tr;            
  C2t3 c2t3(tr);
  Surface_3 surface(implicitFunctionModelMeshGen, Sphere(Point(boundingSphereCenter[0], boundingSphereCenter[1], boundingSphereCenter[2]), boundingSphereRadius* boundingSphereRadius));

  CGAL::Surface_mesh_default_criteria_3<Tr> criteria(30., detail, detail);
  CGAL::make_surface_mesh(c2t3, surface, criteria, CGAL::Non_manifold_tag());
  CGAL::facets_in_complex_2_to_triangle_mesh(c2t3, p->mesh);
  
  init();
}


std::vector<std::vector<glm::dvec3>> Model::slice(const glm::dvec3& normal, double z) {
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

std::vector<std::vector<glm::dvec3>> Model::slice(Model& sliceTool) {
  Polylines polylines;

  Surface_mesh meshCopy = p->mesh;
  //split
  CGAL::Polygon_mesh_processing::split(meshCopy, sliceTool.p->mesh);
  //visit all holes https://doc.cgal.org/latest/Polygon_mesh_processing/Polygon_mesh_processing_2hole_filling_visitor_example_8cpp-example.html
  std::vector<halfedge_descriptor> border_cycles;
  // collect one halfedge per boundary cycle
  PMP::extract_boundary_cycles(meshCopy, std::back_inserter(border_cycles));

  std::vector<std::vector<glm::dvec3>> result;
  for (halfedge_descriptor h : border_cycles)
  {
    std::vector<glm::dvec3> sub;
    halfedge_descriptor current = meshCopy.next(h);
    auto startIndex = meshCopy.source(h);
    auto startPoint = meshCopy.point(startIndex);
    sub.push_back(glm::dvec3(startPoint.x(), startPoint.y(), startPoint.z()));

    while (current != h) {
      auto Index = meshCopy.source(current);
      auto Point = meshCopy.point(Index);
      sub.push_back(glm::dvec3(Point.x(), Point.y(), Point.z()));
      current = meshCopy.next(current);
    }
    result.push_back(sub);
  }

  return result;
}

glm::dvec3 Model::getMin() const {
  return min;
}

glm::dvec3 Model::getMax() const {
  return max;
}

void Model::getBoundingSphere(glm::dvec3& center, double& radius) const {
  center = (min + max) / 2.0;
  radius = glm::distance(min, max); // this is wrong for non cube models (currently i am lazy and will regret it)

  auto points = std::vector<glm::dvec3>{
    min + glm::dvec3(max[0] * 0,max[1] * 0,max[2] * 0),
    min + glm::dvec3(max[0] * 1,max[1] * 0,max[2] * 0),
    min + glm::dvec3(max[0] * 0,max[1] * 1,max[2] * 0),
    min + glm::dvec3(max[0] * 1,max[1] * 1,max[2] * 0),
    min + glm::dvec3(max[0] * 0,max[1] * 0,max[2] * 1),
    min + glm::dvec3(max[0] * 1,max[1] * 0,max[2] * 1),
    min + glm::dvec3(max[0] * 0,max[1] * 1,max[2] * 1),
    min + glm::dvec3(max[0] * 1,max[1] * 1,max[2] * 1)
  };

  for (int a = 0; a < 8; a++) {
    for (int b = a + 1; b < 8; b++) {
      double currentDist = glm::distance(points[a], points[b]);
      if (currentDist > radius)
        radius = currentDist;
    }
  }

  radius /= 2;
}

void Model::repair() {

  std::cout << "Repairing..." << std::endl;
  assert(CGAL::is_valid_polygon_mesh(p->mesh));
  CGAL::Polygon_mesh_processing::remove_isolated_vertices(p->mesh);
  CGAL::Polygon_mesh_processing::duplicate_non_manifold_vertices(p->mesh);
  if (CGAL::Polygon_mesh_processing::does_self_intersect(p->mesh)) {
    std::cout << "Remove Self Intersection..." << std::endl;
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
  //std::cout << "MIN: " << min[0] << "/" << min[1] << "/" << min[2] << std::endl;
  //std::cout << "MAX: " << max[0] << "/" << max[1] << "/" << max[2] << std::endl;

  //std::cout << "Build Tree..." << std::endl;
  p->tree = AABB_tree(edges(p->mesh).first, edges(p->mesh).second, p->mesh);
}

void Model::save(const std::string& filename) {
  CGAL::IO::write_STL(filename, p->mesh);
}

void Model::generateUVMap(const std::string& name) {
  //https://doc.cgal.org/latest/Surface_mesh_parameterization/index.html
  assert(!hasUVMap);
  hasUVMap = true;
  halfedge_descriptor bhd = CGAL::Polygon_mesh_processing::longest_border(p->mesh).first;
  UV_pmap uv_map = p->mesh.add_property_map<vertex_descriptor, Point_2>(name).first;
  SMP::parameterize(p->mesh, bhd, uv_map);
}