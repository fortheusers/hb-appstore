#include "Element.hpp"
#include "TextElement.hpp"
#include "ImageElement.hpp"
#include "ProgressBar.hpp"
#include "Sidebar.hpp"
#include "AppList.hpp"

class MainDisplay : public Element
{
public:
	MainDisplay(Get* get);
	bool process(SDL_Event* event);
	void render(Element* parent);
	void background(int r, int g, int b);
	void update();
	
	Get* get;
	
	bool touchMode = true;
	
private:
	bool showingSplash = true;
	int count = 0;
	
//	// visible GUI child elements of this element
//	std::vector<Element*> elements;
};
