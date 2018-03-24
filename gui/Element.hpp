#include <vector>
#include <SDL/SDL.h>
#include <SDL/SDL_TTF.h>

#pragma once

class Element
{
public:
	// process any input that is received for this element
	virtual bool process(SDL_Event* event);
	
	// display the current state of the display
	virtual void render(Element* parent);
	
	// visible GUI child elements of this element
	std::vector<Element*> elements;
	
	// position the element (x and y are percents of the screen)
	void position(int x, int y);
	
	// remove all elements and free their memory
	void wipeElements();
	
	// SDL main graphics variables
	SDL_Surface* window_surface = NULL;
	
	// whether or not this element is currently being dragged
	bool dragging = false;
	
	// the last Y coordinate of the mouse (from a drag probably)
	int lastMouseY = 0;
	
	// the parent element (can sometimes be null if it isn't set)
	Element* parent = NULL;
	
	typedef Element super;
	
	int x = 0;
	int y = 0;
};
