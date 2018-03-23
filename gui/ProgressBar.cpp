#include "ProgressBar.hpp"
#include <SDL/SDL_gfxPrimitives.h>

void ProgressBar::render(Element* parent)
{
	SDL_Rect location;
	int x = this->x + parent->x;
	int y = this->y + parent->y;
		
	// total width of full progress bar
	int width = 450;
	
	int blue = 0x56c1dfff;
	int gray = 0x989898ff;
	int blue2 = SDL_MapRGB(parent->window_surface->format, 0x56, 0xc1, 0xdf);
	int gray2 = SDL_MapRGB(parent->window_surface->format, 0x98, 0x98, 0x98);
	
	// draw full grayed out bar first
	SDL_Rect gray_rect;
	gray_rect.x = x;
	gray_rect.y = y-4;
	gray_rect.w = width;
	gray_rect.h = 9;
	
	SDL_FillRect(parent->window_surface, &gray_rect, gray2);
	
	// draw ending "circle"
	filledCircleColor(parent->window_surface, x + 450, y, 5, gray);
	
	// draw left "circle" (rounded part of bar)
	filledCircleColor(parent->window_surface, x, y, 5, blue);
	
	// draw blue progress bar so far
	SDL_Rect blue_rect;
	blue_rect.x = x;
	blue_rect.y = y-4;
	blue_rect.w = width*this->percent;
	blue_rect.h = 9;
	
	SDL_FillRect(parent->window_surface, &blue_rect, blue2);
	
	// draw right "circle" (rounded part of bar, and ending)
	filledCircleColor(parent->window_surface, x + width*this->percent, y, 5, blue);
	
	
	// draw right "circle" (rounded part of bar)
//	Draw_FillCircle(parent->window_surface,
//					x, y, 5,
//					blue);
}

