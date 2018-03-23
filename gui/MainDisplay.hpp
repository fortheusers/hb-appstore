#include "Element.hpp"
#include "TextElement.hpp"
#include "ImageElement.hpp"
#include "ProgressBar.hpp"
#include "Sidebar.hpp"

class MainDisplay : public Element
{
public:
	MainDisplay();
	bool process(SDL_Event* event);
	void render(Element* parent);
	void background(int r, int g, int b);
	void update();
	
private:
	bool showingSplash = true;
	int count = 0;
	
//	// visible GUI child elements of this element
//	std::vector<Element*> elements;
};
