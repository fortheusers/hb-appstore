#include "ImageElement.hpp"

ImageElement::ImageElement(const char* path)
{
	this->imgSurface = IMG_Load( path );
}

void ImageElement::render(Element* parent)
{
	SDL_Rect imgLocation;
	imgLocation.x = this->x + parent->x;
	imgLocation.y = this->y + parent->y;
	
	SDL_BlitSurface(this->imgSurface, NULL, parent->window_surface, &imgLocation);
}
