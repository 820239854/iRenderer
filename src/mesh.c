#include <stdio.h>
#include "mesh.h"
#include "array.h"

mesh_t mesh =
{
	.vertices = NULL,
	.faces = NULL,
	.rotation = { 0, 0, 0 }
};

vec3_t cube_vertices[N_CUBE_VERTICES] =
{
	{.x = -1, .y = -1, .z = -1 },
	{.x = -1, .y = 1, .z = -1 },
	{.x = 1, .y = 1, .z = -1 },
	{.x = 1, .y = -1, .z = -1 },
	{.x = 1, .y = 1, .z = 1 },
	{.x = 1, .y = -1, .z = 1 },
	{.x = -1, .y = 1, .z = 1 },
	{.x = -1, .y = -1, .z = 1 }
};

face_t cube_faces[N_CUBE_FACE8] =
{
	{ .a = 1, .b = 2, .c = 3 },
	{ .a = 1, .b = 3, .c = 4 },

	{ .a = 4, .b = 3, .c = 5 },
	{ .a = 4, .b = 5, .c = 6 },

	{ .a = 6, .b = 5, .c = 7 },
	{ .a = 6, .b = 7, .c = 8 },

	{ .a = 8, .b = 7, .c = 2 },
	{ .a = 8, .b = 2, .c = 1 },

	{ .a = 2, .b = 7, .c = 5 },
	{ .a = 2, .b = 5, .c = 3 },

	{ .a = 6, .b = 8, .c = 1 },
	{ .a = 6, .b = 1, .c = 4 }
};

void load_cube_mesh_data(void){
	for(int i=0; i<N_CUBE_FACE8; i++)
	{
		vec3_t cube_vertex = cube_vertices[i];
		array_push(mesh.vertices, cube_vertex);
	}

	for(int i=0; i<N_CUBE_FACE8; i++)
	{
		face_t cube_face = cube_faces[i];
		array_push(mesh.faces, cube_face);
	}
}