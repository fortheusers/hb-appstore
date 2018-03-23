#include <string>
#include "Element.hpp"
#pragma once

class TextElement : public Element
{
public:
	TextElement(const char* text, int size);
	void render(Element* parent);
	SDL_Surface* renderText(std::string& message, int size);

	SDL_Surface* textSurface;

private:
	std::string* text;
	int size;
};
