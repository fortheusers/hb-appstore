#include "Element.hpp"
#include "TextElement.hpp"
#include <SDL2/SDL_image.h>
#pragma once

class Keyboard : public Element
{
public:
	Keyboard();
	void render(Element* parent);
  // bool process(InputEvents* event);
};
