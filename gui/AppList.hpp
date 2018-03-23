#include "TextElement.hpp"
#include "ImageElement.hpp"
#include "../libs/get/src/Get.hpp"

class AppList : public Element
{
public:
	AppList(Get* get);
	bool process();
	void render(Element* parent);
	void update();
	
	Get* get;
	const char* category;
};
