#include "Element.hpp"
#include "TextElement.hpp"
#include <SDL2/SDL_image.h>
#pragma once

class AppList;

class Keyboard : public Element
{
public:
	Keyboard(AppList* appList);
	void render(Element* parent);
  bool process(InputEvents* event);

	// draw a qwerty keyboard
	std::string row1 = "Q W E R T Y U I O P";
	std::string row2 =  "A S D F G H J K L";
	std::string row3 =   "Z X C V B N M";

	std::vector<std::string> rows;

	AppList* appList;
};
