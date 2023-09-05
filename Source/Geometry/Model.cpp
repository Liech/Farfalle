#include "Model.h"

#include <iostream>

#include "Primitives.h"
#include "CGALDefs.h"
#include "Mesh2D.h"
#include <CGAL/IO/polygon_soup_io.h>

typedef PMP::Face_location<Surface_mesh, FT> Face_location;
namespace CP = CGAL::parameters;

class ModelPimpl {
public:
  Surface_mesh mesh;
  AABB_tree    tree;
  UV_pmap      uvmap;
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

Model::Model(const std::vector<glm::dvec3>& vertecies, const std::vector<int>& indices) {
  p = std::make_shared<ModelPimpl>();
  Surface_mesh mesh;
  //https://doc.cgal.org/latest/Surface_mesh/Surface_mesh_2sm_iterators_8cpp-example.html
  std::map<int, vertex_descriptor> vertexMap;
  int counter = 0;
  for (auto& x : vertecies) {
    vertex_descriptor desc = mesh.add_vertex(Point(x.x, x.y, x.z));
    vertexMap[counter] = desc;
    counter++;
  }
  for (int i = 0; i < indices.size(); i+=3) {
    vertex_descriptor a = vertexMap[indices[i + 0]];
    vertex_descriptor b = vertexMap[indices[i + 1]];
    vertex_descriptor c = vertexMap[indices[i + 2]];
    mesh.add_face(a, b, c);
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

std::vector<std::vector<glm::dvec3>> Model::slice2(Model& sliceTool) const {
  //CGAL::Polygon_mesh_processing::surface_intersection	
  Polylines polylines;

  Surface_mesh meshCopy = p->mesh;
  Surface_mesh sliceToolMeshCopy = sliceTool.p->mesh;

  CGAL::Polygon_mesh_processing::surface_intersection(meshCopy, sliceToolMeshCopy,std::back_inserter(polylines));
  
  std::vector<std::vector<glm::dvec3>> result;
  for (const auto& line : polylines) {
    std::vector<glm::dvec3> sub;
    for (const auto& point : line) {
      sub.push_back(glm::dvec3(point.x(), point.y(), point.z()));
    }
    result.push_back(sub);
  }
  return result;
}

std::vector<std::vector<glm::dvec3>> Model::slice(Model& sliceTool) const {
  Polylines polylines;

  Surface_mesh meshCopy = p->mesh;
  Surface_mesh sliceToolMeshCopy = sliceTool.p->mesh;
  //split
  //sliceTool.save("dbg/sliceTool.stl");
  //save("dbg/sliceMe.stl");
  CGAL::Polygon_mesh_processing::clip(meshCopy, sliceToolMeshCopy);
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

std::vector<std::vector<glm::dvec3>> Model::getBorder() const {
  std::vector<halfedge_descriptor> border_cycles;
  PMP::extract_boundary_cycles(p->mesh, std::back_inserter(border_cycles));
  std::vector<std::vector<glm::dvec3>> result;
  for (halfedge_descriptor h : border_cycles)
  {

    std::vector<glm::dvec3> sub;
    halfedge_descriptor current = p->mesh.next(h);
    auto startIndex = p->mesh.source(h);
    auto startPoint = p->mesh.point(startIndex);
    sub.push_back(glm::dvec3(startPoint.x(), startPoint.y(), startPoint.z()));

    while (current != h) {
      auto Index = p->mesh.source(current);
      auto Point = p->mesh.point(Index);
      sub.push_back(glm::dvec3(Point.x(), Point.y(), Point.z()));
      current = p->mesh.next(current);
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

void Model::save(const std::string& filename) const {
  CGAL::IO::write_STL(filename, p->mesh);
}

void Model::generateUVMap() {
  //https://doc.cgal.org/latest/Surface_mesh_parameterization/index.html
  if (hasUVMap)
    return;
  hasUVMap = true;
  halfedge_descriptor bhd = CGAL::Polygon_mesh_processing::longest_border(p->mesh).first;
  p->uvmap = p->mesh.add_property_map<vertex_descriptor, Point_2>("h:UV").first;


  SMP::parameterize(p->mesh, bhd, p->uvmap);
}

double Model::getUVLayerWidth() {
  assert(hasUVMap);

  return 0.4;
}

glm::dvec2 Model::world2UV(const glm::dvec3& pos) const {
  assert(hasUVMap);

  auto onSurface = CGAL::Polygon_mesh_processing::locate(Point(pos.x, pos.y, pos.z), p->mesh);
  auto h1 = p->mesh.halfedge(onSurface.first);
  auto h2 = p->mesh.next(h1);
  auto h3 = p->mesh.next(h2);
  
  vertex_descriptor v1 = p->mesh.source(h1);
  vertex_descriptor v2 = p->mesh.source(h2);
  vertex_descriptor v3 = p->mesh.source(h3); 
  Point_2 uv1 = p->uvmap[v1];
  Point_2 uv2 = p->uvmap[v2];
  Point_2 uv3 = p->uvmap[v3];
  glm::dvec2 guv1 = glm::dvec2(uv1.x(), uv1.y());
  glm::dvec2 guv2 = glm::dvec2(uv2.x(), uv2.y());
  glm::dvec2 guv3 = glm::dvec2(uv3.x(), uv3.y());
  return guv1 * onSurface.second[0] + guv2 * onSurface.second[1] + guv3 * onSurface.second[2];
}

std::vector<glm::dvec3> Model::projectLine(const glm::dvec3& start, const glm::dvec3& end) {
  //https://doc.cgal.org/latest/Surface_mesh_shortest_path/index.html
  Surface_mesh_shortest_path shortest_paths(p->mesh);

  const Point source_pt(start[0],start[1],start[2]);
  const Point target_pt(end[0], end[1], end[2]);
  auto target_loc = shortest_paths.locate<CGAL::AABB_traits<Kernel, CGAL::AABB_face_graph_triangle_primitive<Surface_mesh>>>(source_pt); // this builds an AABB tree of the mesh
  auto source_loc = shortest_paths.locate<CGAL::AABB_traits<Kernel, CGAL::AABB_face_graph_triangle_primitive<Surface_mesh>>>(target_pt); // this builds an AABB tree of the mesh
  // Add the source point
  shortest_paths.add_source_point(source_loc.first, source_loc.second);

  std::vector<Point> points;
  shortest_paths.shortest_path_points_to_source_points(target_loc.first, target_loc.second,
    std::back_inserter(points));
  if (points.size() > 200)
    throw std::runtime_error(":O");

  std::vector<glm::dvec3> result;
  for (auto x : points)
    result.push_back(glm::dvec3(x.x(), x.y(), x.z()));
  return result;
}

glm::dvec3 Model::uv2World(const glm::dvec2& uv) const {
  //https://doc.cgal.org/latest/Polygon_mesh_processing/Polygon_mesh_processing_2locate_example_8cpp-example.html
  //Section: "Locate the same 3D point but in a 2D context"
  Face_location onSurface = PMP::locate(Point_2(uv.x,uv.y), p->mesh, CP::vertex_point_map(p->uvmap));
  auto h1 = p->mesh.halfedge(onSurface.first);
  auto h2 = p->mesh.next(h1);
  auto h3 = p->mesh.next(h2);

  vertex_descriptor v1 = p->mesh.source(h1);
  vertex_descriptor v2 = p->mesh.source(h2);
  vertex_descriptor v3 = p->mesh.source(h3);
  Point p1 = p->mesh.point(v1);
  Point p2 = p->mesh.point(v2);
  Point p3 = p->mesh.point(v3);
  glm::dvec3 g1 = glm::dvec3(p1.x(), p1.y(), p1.z());
  glm::dvec3 g2 = glm::dvec3(p2.x(), p2.y(), p2.z());
  glm::dvec3 g3 = glm::dvec3(p3.x(), p3.y(), p3.z());
  return g1 * onSurface.second[0] + g2 * onSurface.second[1] + g3 * onSurface.second[2];
}

std::vector<glm::dvec3> Model::uv2World(const std::vector<glm::dvec2>& uv)  const{
  std::vector<glm::dvec3> result;
  result.reserve(uv.size());
  for (const auto& x : uv) {
    result.push_back(uv2World(x));
  }
  return result;
}

size_t Model::getNumberFaces() const {
  return p->mesh.number_of_faces();
}

std::array<size_t, 3> Model::getFaceIndices(size_t number) const {
  Surface_mesh::Face_iterator iterator = p->mesh.faces_begin() + number;
  auto descriptor = *iterator;
  auto h1 = p->mesh.halfedge(descriptor);
  auto h2 = p->mesh.next(h1);
  auto h3 = p->mesh.next(h2);
  vertex_descriptor v1 = p->mesh.source(h1);
  vertex_descriptor v2 = p->mesh.source(h2);
  vertex_descriptor v3 = p->mesh.source(h3);
  return { v1.idx(),v2.idx(),v3.idx() };
}

glm::dvec3 Model::getVertexPosition(size_t idx) const {
  Point r = p->mesh.point(CGAL::SM_Vertex_index(idx));
  return glm::dvec3(r.x(), r.y(), r.z());
}

std::unique_ptr<Model> Model::from2D(const Mesh2D& mesh2d) const {
  std::unique_ptr<Model> result = std::make_unique<Model>();
  auto& mesh = result->p->mesh;
  std::vector<Point> points;
  std::vector<std::array<size_t, 3>> faces;
  std::vector<CGAL::SM_Vertex_index> indices;
  indices.reserve(points.size());

  

  for (size_t i = 0; i < points.size(); i++) {    
    indices.push_back(mesh.add_vertex(points[i]));
  }
  for (size_t i = 0; i < faces.size(); i++) {
    auto& face = faces[i];
    mesh.add_face(indices[face[0]], indices[face[1]], indices[face[2]]);
  }
  return std::move(result);
}