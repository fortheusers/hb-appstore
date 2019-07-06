#include "Element.hpp"
#include "ImageCache.hpp"
#include <SDL2/SDL.h>
#pragma once

enum ImageScaleMode
{
	// Stretch the image to fill the size
	IMAGE_STRETCH,

	// Keep image proportions and fill the background with first pixel's color
	IMAGE_SCALE_BG,
};

class Image : public Element
{
public:
	virtual ~Image();

	// Loads the image from a surface
	// Returns true if successful
	bool loadFromSurface(SDL_Surface *surface);

	// Loads the image from caches
	// Returns true if successful
	bool loadFromCache(std::string &key);

	// Loads the image from a surface and saves the results in caches
	// Returns true if successful
	bool loadFromSurfaceSaveToCache(std::string &key, SDL_Surface *surface);

	// Renders the image
	void render(Element* parent);

	// Resizes the image
	void resize(int w, int h);

	// Sets image scaling mode
	void setScaleMode(ImageScaleMode mode);

protected:
	// The actual texture of the image
	SDL_Texture *imgTexture = nullptr;

	// The size of the texture
	int imgW = 0, imgH = 0;

	// The color of the first pixel
	SDL_Color imgFirstPixel = {0,0,0,0};

	// Image's scaling mode
	ImageScaleMode imgScaleMode = IMAGE_STRETCH;
};
