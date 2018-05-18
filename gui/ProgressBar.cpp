#include "ProgressBar.hpp"
#include <SDL2/SDL2_gfxPrimitives.h>

ProgressBar::ProgressBar()
{
	// total width of full progress bar
	this->width = 450;
	this->color = 0x56c1dfff;
}

void ProgressBar::render(Element* parent)
{
	SDL_Rect location;
	int x = this->x + parent->x;
	int y = this->y + parent->y;

	int blue = this->color;
	int gray = 0x989898ff;
	int blue2 = SDL_MapRGB(parent->window_surface->format, (blue >> 24) & 0xff, (blue >> 16) & 0xff, (blue >> 8) & 0xff);
	int gray2 = SDL_MapRGB(parent->window_surface->format, 0x98, 0x98, 0x98);

	// draw full grayed out bar first
	SDL_Rect gray_rect;
	gray_rect.x = x;
	gray_rect.y = y-4;
	gray_rect.w = width;
	gray_rect.h = 9;

	SDL_FillRect(parent->window_surface, &gray_rect, gray2);

	// draw ending "circle"
	filledCircleColor(parent->renderer, x + this->width, y, 5, gray);

	// draw left "circle" (rounded part of bar)
	filledCircleColor(parent->renderer, x, y, 5, blue);

	// draw blue progress bar so far
	SDL_Rect blue_rect;
	blue_rect.x = x;
	blue_rect.y = y-4;
	blue_rect.w = width*this->percent;
	blue_rect.h = 9;

	SDL_FillRect(parent->window_surface, &blue_rect, blue2);

	// draw right "circle" (rounded part of bar, and ending)
	filledCircleColor(parent->renderer, x + width*this->percent, y, 5, blue);

}
