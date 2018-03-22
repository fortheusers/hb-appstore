#include "ImageElement.hpp"

ImageElement::ImageElement(const char* path)
{
	this->imgSurface = IMG_Load( path );
}

void ImageElement::render(Element* parent)
{
	SDL_Rect textLocation = { 0, 0, 0, 0 };
	
	SDL_BlitSurface(this->imgSurface, NULL, parent->window_surface, &textLocation);
}
