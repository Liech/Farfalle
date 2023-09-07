#pragma once

#include <memory>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point_3;

#include "CGAL/Surface_mesh.h"
typedef CGAL::Surface_mesh<Point_3> Surface_Mesh;

#include "CGAL/Nef_polyhedron_3.h"
typedef CGAL::Nef_polyhedron_3<Kernel> NefPolyhedron;

class Model;

class VolumeImplementation {
public:
  VolumeImplementation(Surface_Mesh&);
  virtual ~VolumeImplementation();

  std::unique_ptr<Model> getModel() const;

private:
  std::unique_ptr<NefPolyhedron> nefRepresentation;
};