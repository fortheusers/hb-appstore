#include <string>
#include "Element.hpp"
#pragma once

class TextElement : public Element
{
public:
	TextElement(const char* text, int size, SDL_Color* color = 0);
	void render(Element* parent);
	SDL_Texture* renderText(std::string& message, int size);


	int width, height;

	SDL_Texture* textSurface = NULL;
	SDL_Color color;

private:
	std::string* text;
	int size;
};
