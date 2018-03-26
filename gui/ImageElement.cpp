#include "MainDisplay.hpp"

ImageElement::ImageElement(const char* path)
{
	std::string key = std::string(path);
	
	// try to find it in the cache first
	if (MainDisplay::cache.count(key))
	{
		this->imgSurface = &MainDisplay::cache[key];
		return;
	}
	
	// not found, create it
	
	if (this->imgSurface != NULL)
		SDL_FreeSurface(this->imgSurface);
	
	this->imgSurface = IMG_Load( path );
	
	// add to cache for next time
	MainDisplay::cache[key] = *(this->imgSurface);
}

void ImageElement::render(Element* parent)
{
	SDL_Rect imgLocation;
	imgLocation.x = this->x + parent->x;
	imgLocation.y = this->y + parent->y;
	
	SDL_BlitSurface(this->imgSurface, NULL, parent->window_surface, &imgLocation);
}

void ImageElement::resize(int width, int height)
{
	// TODO: when moving to SDL2, do a resize here
}
