#pragma once

#include "vector.h"
#include "triangle.h"

#define N_MESH_VERTICES 8
extern vec3_t mesh_vertices[N_MESH_VERTICES];

#define N_MESH_FACE8 (6*2)
extern face_t mesh_faces[N_MESH_FACE8];