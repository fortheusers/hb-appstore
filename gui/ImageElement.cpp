#include "MainDisplay.hpp"
#include <SDL2/SDL2_rotozoom.h>
#include <string.h>

ImageElement::ImageElement(const char* path)
{
	std::string key = std::string(path);
	this->path = path;
#if defined(SWITCH)
	// if the image path we want to load starts with 'res', prepend romfs for switch
	if (strncmp(this->path, "res/", 4) == 0)
    this->path = (std::string("romfs:/") + path).c_str();
#endif

	// try to find it in the cache first
	if (ImageCache::cache.count(key))
	{
		this->imgSurface = ImageCache::cache[key];
		return;
	}

	// not found, create it

	if (this->imgSurface != NULL)
		SDL_DestroyTexture(this->imgSurface);

	SDL_Surface* surface = IMG_Load(this->path);
	this->imgSurface = SDL_CreateTextureFromSurface(MainDisplay::mainRenderer, surface);

	this->width = 100; //surface->w;
	this->height = 100; //surface->h;

	SDL_FreeSurface(surface);

	// add to cache for next time
	if (this->imgSurface != NULL)
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
	this->width = width;
	this->height = height;
}
