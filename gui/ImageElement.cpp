#include "MainDisplay.hpp"
#include <SDL/SDL_rotozoom.h>

ImageElement::ImageElement(const char* path)
{
	std::string key = std::string(path);
	this->path = path;
	
	// try to find it in the cache first
	if (ImageCache::cache.count(key))
	{
		this->imgSurface = &ImageCache::cache[key];
		return;
	}
	
	// not found, create it
	
	if (this->imgSurface != NULL)
		SDL_FreeSurface(this->imgSurface);
	
	this->imgSurface = IMG_Load( path );
	
	// add to cache for next time
	if (this->imgSurface != NULL)
		ImageCache::cache[key] = *(this->imgSurface);
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
	if (width == this->imgSurface->w && height == this->imgSurface->h)
		return;		// already the right size
	
	SDL_Surface* delme = this->imgSurface;

	this->imgSurface = rotozoomSurfaceXY(this->imgSurface, 0, ((double)width)/this->imgSurface->w, ((double)height)/this->imgSurface->h, 1);

	// delete the old surface, and update the cache
	ImageCache::cache[this->path] = *(this->imgSurface);
}
