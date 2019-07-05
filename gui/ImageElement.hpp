#pragma once
#include "Element.hpp"
#include <SDL2/SDL_image.h>

class ImageElement : public Element
{
public:
	ImageElement(const char* path);
	void render(Element* parent);

	SDL_Texture* imgSurface = nullptr;
	const char* path;

	void resize(int width, int height);
};
