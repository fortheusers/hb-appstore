#include "MainDisplay.hpp"
#include <SDL2/SDL2_rotozoom.h>
#include <string.h>

ImageElement::ImageElement(const char* incoming)
{
	this->path = incoming;

	std::string key = std::string(this->path);

	// try to find it in the cache first
	if (ImageCache::cache.count(key))
	{
		this->imgSurface = ImageCache::cache[key];
		return;
	}

	// not found, create it

	SDL_Surface* surface = IMG_Load(this->path);

	this->imgSurface = SDL_CreateTextureFromSurface(MainDisplay::mainRenderer, surface);

	this->width = 0;  //surface->w;
	this->height = 0; //surface->h;

	SDL_FreeSurface(surface);

	// add to cache for next time
	if (this->imgSurface)
		ImageCache::cache[key] = (this->imgSurface);
}

void ImageElement::render(Element* parent)
{
	SDL_Rect imgLocation;
	imgLocation.x = this->x + parent->x;
	imgLocation.y = this->y + parent->y;
	imgLocation.w = this->width;
	imgLocation.h = this->height;

	SDL_RenderCopy(MainDisplay::mainRenderer, this->imgSurface, NULL, &imgLocation);
}

void ImageElement::resize(int width, int height)
{
	// don't resize for null image surfaces
	if (this->imgSurface == NULL)
		return;

	this->width = width;
	this->height = height;
}
