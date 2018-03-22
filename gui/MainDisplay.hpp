#include  "Element.hpp"

class MainDisplay : public Element
{
public:
	MainDisplay();
	bool processInput(SDL_Event* event);
	void render();
	void background(int r, int g, int b);
	void update();
	
private:
	// SDL graphics variables
	SDL_Surface* window_surface;
	bool showingSplash = true;
	
//	// visible GUI child elements of this element
//	std::vector<Element*> elements;
};
