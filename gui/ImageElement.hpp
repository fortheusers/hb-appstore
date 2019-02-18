#include "Element.hpp"
#include <SDL2/SDL_image.h>
#pragma once

class ImageElement : public Element
{
public:
	ImageElement(const char* path, bool calcFirstPixel = false);
	void render(Element* parent);

	SDL_Texture* imgSurface = NULL;
	const char* path;

	SDL_Color* firstPixel = NULL;

	void resize(int width, int height);
};
