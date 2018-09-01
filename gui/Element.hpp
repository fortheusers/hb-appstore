#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "InputEvents.hpp"

#pragma once

class Element
{
public:
	// process any input that is received for this element
	virtual bool process(InputEvents* event);

	// display the current state of the display
	virtual void render(Element* parent);

	// hide the element
	void hide();

	// visible GUI child elements of this element
	std::vector<Element*> elements;

	// position the element (x and y are percents of the screen)
	void position(int x, int y);

	// remove all elements and free their memory
	void wipeElements();
    
    // the scale of the element (and its subelements!)
    float scale = 1.0f;

	// SDL main graphics variables
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	// whether or not this element is currently being dragged
	bool dragging = false;

	// the last Y coordinate of the mouse (from a drag probably)
	int lastMouseY = 0;

	// the parent element (can sometimes be null if it isn't set)
	Element* parent = NULL;

	// whether this element should skip rendering or not
	bool hidden = false;

	// how much time is left in an elastic-type flick/scroll
	// set by the last distance traveled in a scroll, and counts down every frame
	int elasticCounter = 0;

	typedef Element super;

	int x = 0;
	int y = 0;
};
