#include <vector>
#include <SDL/SDL.h>
#include <SDL/SDL_TTF.h>

#pragma once

class Element
{
public:
	// process any input that is received for this element
	virtual bool processInput(SDL_Event* event);
	
	// display the current state of the display
	virtual void render(Element* parent);
	
	// visible GUI child elements of this element
	std::vector<Element*> elements;
	
	// position the element (x and y are percents of the screen)
	void position(float x, float y);
	
	// SDL main graphics variables
	SDL_Surface* window_surface;
	
	typedef Element super;
	
	int x;
	int y;
};
