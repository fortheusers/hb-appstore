#include "TextElement.hpp"
#include "ImageElement.hpp"
#include "AppPopup.hpp"
#include "Sidebar.hpp"
#include "InertiaScroll.hpp"
#include "../libs/get/src/Get.hpp"

class AppList : public Element
{
public:
	AppList(Get* get, Sidebar* sidebar);
	bool process(InputEvents* event);
	void render(Element* parent);
	void update();

	Get* get = NULL;
	Sidebar* sidebar = NULL;

	int highlighted = -1;
	bool touchMode = true;

	// the total number of apps displayed in this list
	int totalCount = 0;

	// the currently displayed subscreen (NULL otherwise)
	AppPopup* subscreen = NULL;

};
