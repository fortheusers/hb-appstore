#include "MainDisplay.hpp"
#include <SDL2/SDL2_rotozoom.h>
#include <string.h>

Image::~Image()
{
}

bool Image::loadFromSurface(SDL_Surface *surface)
{
	if (!surface)
		return false;

	// try to create a texture from the surface
	SDL_Texture *texture = SDL_CreateTextureFromSurface(MainDisplay::mainRenderer, surface);
	if (!texture)
		return false;

	// load first pixel color
	Uint32 pixelcolor = 0;
	for (int i = 0; i < surface->format->BytesPerPixel; i++)
		pixelcolor = (pixelcolor << 8) + *((Uint8*)surface->pixels + i);
	SDL_GetRGB(pixelcolor, surface->format, &imgFirstPixel.r, &imgFirstPixel.g, &imgFirstPixel.b);

	// load texture size
	SDL_QueryTexture(texture, nullptr, nullptr, &imgW, &imgH);

	// load texture
	imgTexture = texture;

	return true;
}

bool Image::loadFromCache(std::string &key)
{
	// check if the image is cached
	if (ImageCache::imageCache.count(key))
	{
		ImageData *imgData = &ImageCache::imageCache[key];
		imgTexture = imgData->texture;
		imgFirstPixel = imgData->firstPixel;
		SDL_QueryTexture(imgTexture, nullptr, nullptr, &imgW, &imgH);
		return true;
	}

	return false;
}

bool Image::loadFromSurfaceSaveToCache(std::string &key, SDL_Surface *surface)
{
	bool success = loadFromSurface(surface);

	// only save to caches if loading was successful
	// and the image isn't already cached
	if (success && !ImageCache::imageCache.count(key))
	{
		ImageData imgData;
		imgData.texture = imgTexture;
		imgData.firstPixel = imgFirstPixel;
		ImageCache::imageCache[key] = imgData;
	}

	return success;
}

void Image::render(Element* parent)
{
	if (!imgTexture)
		return;

	// rect of element's size
	SDL_Rect rect;
	rect.x = x + parent->x;
	rect.y = y + parent->y;
	rect.w = width;
	rect.h = height;

	if (imgScaleMode == IMAGE_SCALE_BG)
	{
		// draw colored background
		SDL_SetRenderDrawColor(MainDisplay::mainRenderer, imgFirstPixel.r, imgFirstPixel.g, imgFirstPixel.b, 0xFF);
		SDL_RenderFillRect(MainDisplay::mainRenderer, &rect);

		// recompute drawing rect
		if ((width * imgH) > (height * imgW))
		{
			// keep height, scale width
			rect.h = height;
			rect.w = (imgW * rect.h) / imgH;
		}
		else
		{
			// keep width, scale height
			rect.w = width;
			rect.h = (imgH * rect.w) / imgW;
		}

		// center the texture
		rect.x += (width - rect.w) / 2;
		rect.y += (height - rect.h) / 2;
	}

	// render the texture
	SDL_RenderCopy(MainDisplay::mainRenderer, imgTexture, NULL, &rect);
}

void Image::resize(int w, int h)
{
	width = w;
	height = h;
}

void Image::setScaleMode(ImageScaleMode mode)
{
	imgScaleMode = mode;
}
