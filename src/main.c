#include <windows.h>

#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>
#include<SDL.h>

bool windowed = true;
bool is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;

bool initialize_window(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error init SDL");
		return false;
	}

	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width,
		window_height,
		SDL_WINDOW_BORDERLESS
	);

	if (!window)
	{
		if (!window)
			fprintf(stderr, "Error init window");
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer)
	{
		if (!window)
			fprintf(stderr, "Error init renderer");
		return false;
	}

	if(!windowed)
	{
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	}

	return true;
}

void setup(void)
{
	color_buffer = (uint32_t*)malloc(window_width*window_height*sizeof(uint32_t));

	color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
	                                         SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
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

void update(void)
{

}

void clear_color_buffer(uint32_t color)
{
	for(int y=0; y<window_height; y++)
	{
		for(int x=0; x<window_width; x++)
		{
			color_buffer[y*window_width + x] = color;
		}
	}
}

void render_color_buffer(void)
{
	SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer, (int)(sizeof(uint32_t)*window_width));
	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void draw_grid(uint32_t color)
{
	for(int y=0; y<window_height; y++)
	{
		for(int x=0; x<window_width; x++)
		{
			if (x % 10 == 0 || y % 10 == 0)
			{
				color_buffer[y*window_width + x] = color;
			}
		}
	}
}

void draw_rect(int x, int y,int width, int height, uint32_t color)
{
	for (int curr_y = y; curr_y < y + height; curr_y++)
	{
		for (int curr_x = x; curr_x < x + width; curr_x++)
		{
			color_buffer[curr_y*window_width + curr_x] = color;
		}
	}
}

void render(void)
{
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderClear(renderer);

	//draw_grid(0xFFFFFFFF);
	draw_rect(100, 100, 100, 100, 0xFFFF0000);

	render_color_buffer();
	clear_color_buffer(0xFF000000);

	SDL_RenderPresent(renderer);
}

void destroy_window(void)
{
	free(color_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
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