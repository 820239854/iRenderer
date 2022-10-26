#include <windows.h>
#include "display.h"
#include "vector.h"

float fov_factor = 640;
bool is_running = false;

int previoous_frame_time = 0;

#define N_POINTS (9*9*9)

vec3_t camera_pos = { 0, 0, -5 };
vec3_t cube_rotation = { 0, 0, 0 };

vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

void setup(void)
{
	color_buffer = (uint32_t*)malloc(window_width*window_height*sizeof(uint32_t));
	color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                                         SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
	int point_count = 0;
	for (float x = -1; x <= 1; x += 0.25)
	{
		for (float y = -1; y <= 1; y += 0.25)
		{
			for (float z = -1; z <= 1; z += 0.25)
			{
				vec3_t new_point = { x, y, z };
				cube_points[point_count] = new_point;
				point_count++;
			}
		}
	}
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
	if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
	{
		SDL_Delay(time_to_wait);
	}
	previoous_frame_time = SDL_GetTicks();

	cube_rotation.z += 0.01;

	for(int i=0; i<N_POINTS; i++)
	{
		vec3_t point = vec3_rotate_z(cube_points[i], cube_rotation.z);
		point.z -= camera_pos.z;
		projected_points[i] = project(point);
	}
}

void render(void)
{
	clear_color_buffer(0xFF000000);
	
	for(int i=0; i<N_POINTS; i++)
	{
		vec2_t proj_point = projected_points[i];
		draw_rect(proj_point.x + window_width / 2, proj_point.y + window_height / 2,
		          4, 4, 0XFF00FF00);
	}
	render_color_buffer();
	SDL_RenderPresent(renderer);
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
	return 0;
}