#include <vector>
#include <SDL/SDL.h>

class Element
{
public:
	// process any input that is received for this element
	virtual bool processInput(SDL_Event* event);
	
	// display the current state of the display
	virtual void render();
	
	// visible GUI child elements of this element
	std::vector<Element*> elements;
	
	typedef Element super;
};
