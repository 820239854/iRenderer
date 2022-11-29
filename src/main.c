#include <windows.h>
#include "display.h"
#include "vector.h"
#include "array.h"
#include "mesh.h"
#include "matrix.h"

triangle_t *triangles_to_render = NULL;

float fov_factor = 640;
bool is_running = false;
int previoous_frame_time = 0;

vec3_t camera_pos = { 0, 0, 0 };
mat4_t proj_matrix;

void setup(void)
{
	render_method = RENDER_WIRE;
	cull_method = CULL_BACKFACE;
	color_buffer = (uint32_t*)malloc(window_width*window_height*sizeof(uint32_t));
	color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                                         SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
	// Initialize the perspective projection matrix
	float aspect_y = (float)window_height / (float)window_width;
	float aspect_x = (float)window_width / (float)window_height;
	float fov_y = M_PI / 3.0; // the same as 180/3, or 60deg
	float fov_x = atan(tan(fov_y / 2) * aspect_x) * 2;
	float znear = 1.0;
	float zfar = 100.0;
	proj_matrix = mat4_make_perspective(fov_y, aspect_y, znear, zfar);

	//load_obj_file_data("./assets/cube.obj");
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
			if (event.key.keysym.sym == SDLK_1)
				render_method = RENDER_WIRE_VERTEX;
			if (event.key.keysym.sym == SDLK_2)
				render_method = RENDER_WIRE;
			if (event.key.keysym.sym == SDLK_3)
				render_method = RENDER_FILL_TRIANGLE;
			if (event.key.keysym.sym == SDLK_4)
				render_method = RENDER_FILL_TRIANGLE_WIRE;
			if (event.key.keysym.sym == SDLK_c)
				cull_method = CULL_BACKFACE;
			if (event.key.keysym.sym == SDLK_d)
				cull_method = CULL_NONE;
			break;
	}
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
	//mesh.scale.x += 0.002;
	//mesh.scale.y += 0.001;
	//mesh.translation.x += 0.01;
	mesh.translation.z = 5.0;

	mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
	mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
	mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
	mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

	int num_faces = array_length(mesh.faces);
	for (int i = 0; i < num_faces; i++)
	{
		face_t mesh_face = mesh.faces[i];

		vec3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face.a - 1];
		face_vertices[1] = mesh.vertices[mesh_face.b - 1];
		face_vertices[2] = mesh.vertices[mesh_face.c - 1];

		vec4_t transformed_vertices[3];
		for (int j = 0; j < 3; j++)
		{
			mat4_t world_matrix = mat4_identity();
			world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
			world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

			vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);
			transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);
			transformed_vertices[j] = transformed_vertex;
		}
		if (cull_method == CULL_BACKFACE)
		{
			vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
			vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
			vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);

			vec3_t vector_ab = vec3_sub(vector_b, vector_a);
			vec3_t vector_ac = vec3_sub(vector_c, vector_a);
			vec3_t normal = vec3_cross(vector_ab, vector_ac);
			vec3_normalize(&normal);

			vec3_t camera_ray = vec3_sub(camera_pos, vector_a);
			float dot_normal_camera = vec3_dot(normal, camera_ray);
			if(dot_normal_camera < 0)
			{
				continue;
			}
		}
		vec4_t projected_points[3];
		for(int j = 0; j < 3; j++)
		{
			projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);
			projected_points[j].x *= (window_width / 2);
			projected_points[j].y *= (window_height / 2);

			projected_points[j].x += (window_width / 2);
			projected_points[j].y += (window_height / 2);
		}
		float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;
		triangle_t projected_triangle = {
			.points =
			{
				{ projected_points[0].x, projected_points[0].y },
				{ projected_points[1].x, projected_points[1].y },
				{ projected_points[2].x, projected_points[2].y },
			},
			.color = mesh_face.color,
			.avg_depth = avg_depth
		};
		array_push(triangles_to_render, projected_triangle);

		int num_triangles = array_length(triangles_to_render);
		for (int i = 0; i < num_triangles; i++)
		{
			for (int j = i; j < num_triangles; j++)
			{
				if(triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth)
				{
					triangle_t tmp = triangles_to_render[i];
					triangles_to_render[i] = triangles_to_render[j];
					triangles_to_render[j] = tmp;
				}
			}
		}
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
		if(render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE)
		{
			draw_filled_triangle(
				points[0].x, points[0].y, 
				points[1].x, points[1].y, 
				points[2].x, points[2].y, 
				triangle.color
			);
		}
		if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE)
		{
			draw_triangle(points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y, 0xFFFFFFFF);
		}
		if(render_method == RENDER_WIRE_VERTEX)
		{
			draw_rect(points[0].x-3, points[0].y-3, 6, 6, 0xFFFF0000);
			draw_rect(points[1].x-3, points[1].y-3, 6, 6, 0xFFFF0000);
			draw_rect(points[2].x-3, points[2].y-3, 6, 6, 0xFFFF0000);
		}
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