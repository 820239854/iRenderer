#include "display.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;

bool windowed = true;

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

	if (!windowed)
	{
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	}

	return true;
}

void clear_color_buffer(uint32_t color)
{
	for (int y = 0; y < window_height; y++)
	{
		for (int x = 0; x < window_width; x++)
		{
			color_buffer[y * window_width + x] = color;
		}
	}
}

void render_color_buffer(void)
{
	SDL_UpdateTexture(color_buffer_texture, NULL, color_buffer, (int)(sizeof(uint32_t) * window_width));
	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void draw_pixel(int x, int y, uint32_t color)
{
	if (x >= 0 && x<window_width && y>=0 && y<window_height)
	{
		color_buffer[y * window_width + x] = color;
	}
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color)
{
	int delta_x = x1 - x0;
	int delta_y = y1 - y0;
	int longest_side_length = (abs(delta_x) >= abs(delta_y))? abs(delta_x):abs(delta_y);

	float x_inc = delta_x / (float)longest_side_length;
	float y_inc = delta_y / (float)longest_side_length;

	float curr_x = x0;
	float curr_y = y0;
	for(int i=0; i<=longest_side_length; i++)
	{
		draw_pixel(round(curr_x), round(curr_y), color);
		curr_x += x_inc;
		curr_y += y_inc;
	}
}

void draw_triangle(int x0,int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
	draw_line(x0, y0, x1, y1, color);
	draw_line(x1, y1, x2, y2, color);
	draw_line(x2, y2, x0, y0, color);
}

void draw_grid(uint32_t color)
{
	for (int y = 0; y < window_height; y++)
	{
		for (int x = 0; x < window_width; x++)
		{
			if (x % 10 == 0 || y % 10 == 0)
			{
				color_buffer[y * window_width + x] = color;
			}
		}
	}
}

void draw_rect(int x, int y, int width, int height, uint32_t color)
{
	for (int curr_y = y; curr_y < y + height; curr_y++)
	{
		for (int curr_x = x; curr_x < x + width; curr_x++)
		{
			draw_pixel(curr_x, curr_y, color);
		}
	}
}

void destroy_window(void)
{
	free(color_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}