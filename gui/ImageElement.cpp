#include "MainDisplay.hpp"
#include <SDL2/SDL2_rotozoom.h>

ImageElement::ImageElement(const char* path)
{
	std::string key = std::string(path);
	this->path = path;

	// try to find it in the cache first
	if (ImageCache::cache.count(key))
	{
		this->imgSurface = ImageCache::cache[key];
		return;
	}

	// not found, create it

	if (this->imgSurface != NULL)
		SDL_DestroyTexture(this->imgSurface);

	this->imgSurface = IMG_LoadTexture(this->renderer, path );

	// add to cache for next time
	if (this->imgSurface != NULL)
		ImageCache::cache[key] = (this->imgSurface);
}

void ImageElement::render(Element* parent)
{
	SDL_Rect imgLocation;
	imgLocation.x = this->x + parent->x;
	imgLocation.y = this->y + parent->y;

	SDL_RenderCopy(parent->renderer, this->imgSurface, NULL, &imgLocation);
}

void ImageElement::resize(int width, int height)
{
	int w, h;
	SDL_QueryTexture(this->imgSurface, NULL, NULL, &w, &h);
	
	if (width == w && height == h)
		return;		// already the right size

	SDL_Texture* delme = this->imgSurface;

//    this->imgSurface = rotozoomSurfaceXY(this->imgSurface, 0, ((double)width)/this->imgSurface->w, ((double)height)/this->imgSurface->h, 1);

	// delete the old surface, and update the cache
	ImageCache::cache[this->path] = (this->imgSurface);
}
