#include "Element.hpp"
#include <SDL/SDL_image.h>
#pragma once

class ImageElement : public Element
{
public:
	ImageElement(const char* path);
	void render(Element* parent);
	
	SDL_Surface* imgSurface = NULL;
	
	void resize(int width, int height);
};

