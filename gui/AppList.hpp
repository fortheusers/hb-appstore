#include "TextElement.hpp"
#include "ImageElement.hpp"
#include "../libs/get/src/Get.hpp"

class AppList : public Element
{
public:
	AppList(Get* get);
	bool process(SDL_Event* event);
	void render(Element* parent);
	void update();
	
	Get* get;
	const char* category;
	
	// how much time is left in an elastic-type flick/scroll
	// set by the last distance traveled in a scroll, and counts down every frame
	int elasticCounter;
};
