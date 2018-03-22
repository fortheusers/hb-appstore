#include "Element.hpp"

bool Element::processInput(SDL_Event* event)
{
	
	
	return false;
}

void Element::render()
{
	for (int x=0; x<this->elements.size(); x++)
	{
		// go through every subelement and run render
		this->elements[x]->render();
	}
}

