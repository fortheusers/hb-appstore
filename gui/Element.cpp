#include "Element.hpp"

bool Element::processInput(SDL_Event* event)
{
	
	
	return false;
}

void Element::render(Element* parent)
{
	for (int x=0; x<this->elements.size(); x++)
	{
		// go through every subelement and run render
		this->elements[x]->render(parent);
	}
}

void Element::position(int x, int y)
{
	this->x = x;
	this->y = y;
}
