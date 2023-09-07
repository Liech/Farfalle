#pragma once

#include <memory>

#include "CGALDefs.h"

class ModelImplementation{
	public:
    ModelImplementation();
    virtual ~ModelImplementation();

    Surface_mesh mesh;
    AABB_tree    tree;
    UV_pmap      uvmap;
};