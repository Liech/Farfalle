#pragma once

#include <memory>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point_3;

#include "CGAL/Surface_mesh.h"
typedef CGAL::Surface_mesh<Point_3> Surface_Mesh;

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
typedef CGAL::Exact_predicates_exact_constructions_kernel EKernel;
typedef EKernel::Point_3 EPoint_3;

#include "CGAL/Surface_mesh.h"
typedef CGAL::Surface_mesh<EPoint_3> ESurface_Mesh;

#include "CGAL/Nef_polyhedron_3.h"
typedef CGAL::Nef_polyhedron_3<EKernel> NefPolyhedron;

class Model;

class VolumeImplementation {
public:
  VolumeImplementation(Surface_Mesh&);
  virtual ~VolumeImplementation();

  std::unique_ptr<Model> getModel() const;
  void minkowski(VolumeImplementation&);

private:
  std::unique_ptr<NefPolyhedron> nefRepresentation;

  std::unique_ptr<ESurface_Mesh> convert(Surface_Mesh&)     const;
  std::unique_ptr<Surface_Mesh> convertBack(ESurface_Mesh&) const;
};