#include <windows.h>
#include "display.h"
#include "vector.h"
#include "array.h"
#include "mesh.h"

triangle_t *triangles_to_render = NULL;

float fov_factor = 640;
bool is_running = false;

int previoous_frame_time = 0;

vec3_t camera_pos = { 0, 0, -5 };

void setup(void)
{
	color_buffer = (uint32_t*)malloc(window_width*window_height*sizeof(uint32_t));
	color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                                         SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
	load_cube_mesh_data();
}

void process_input(void)
{
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type)
	{
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
				is_running = false;
			break;
	}
}

vec2_t project(vec3_t point)
{
	vec2_t projected_point =
	{
		fov_factor * point.x / point.z,
		fov_factor * point.y / point.z
	};
	return projected_point;
}

void update(void)
{
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() -previoous_frame_time);
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
	{
		SDL_Delay(time_to_wait);
	}
	previoous_frame_time = SDL_GetTicks();

	triangles_to_render = NULL;

	mesh.rotation.x += 0.01;
	mesh.rotation.y += 0.01;
	mesh.rotation.z += 0.01;

	int num_faces = array_length(mesh.faces);
	for (int i = 0; i < num_faces; i++)
	{
		face_t mesh_face = mesh.faces[i];

		vec3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face.a - 1];
		face_vertices[1] = mesh.vertices[mesh_face.b - 1];
		face_vertices[2] = mesh.vertices[mesh_face.c - 1];

		triangle_t projected_triangle;

		for (int j = 0; j < 3; j++)
		{
			vec3_t transformed_vertex = face_vertices[j];

			transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
			transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
			transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);
			transformed_vertex.z -= camera_pos.z;
			
			vec2_t projected_point = project(transformed_vertex);

			projected_point.x += (window_width / 2);
			projected_point.y += (window_height / 2);

			projected_triangle.points[j] = projected_point;
		}

		array_push(triangles_to_render, projected_triangle);
	}
}

void render(void)
{
	clear_color_buffer(0xFF000000);

	int num_triangles = array_length(triangles_to_render);
	for (int i = 0; i < num_triangles; i++)
	{
		triangle_t triangle = triangles_to_render[i];
		vec2_t points[3];
		points[0] = triangle.points[0];
		points[1] = triangle.points[1];
		points[2] = triangle.points[2];

		draw_rect(points[0].x, points[0].y, 3, 3, 0xFFFFFFFF);
		draw_rect(points[1].x, points[1].y, 3, 3, 0xFFFFFFFF);
		draw_rect(points[2].x, points[2].y, 3, 3, 0xFFFFFFFF);

		draw_triangle(points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y, 0xFF00FF00);
	}

	array_free(triangles_to_render);
	render_color_buffer();
	SDL_RenderPresent(renderer);
}

void free_resources(void)
{
	free(color_buffer);
	array_free(mesh.faces);
	array_free(mesh.vertices);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	is_running = initialize_window();

	setup();
	while (is_running)
	{
		process_input();
		update();
		render();
	}
	destroy_window();
	free_resources();
	return 0;
}