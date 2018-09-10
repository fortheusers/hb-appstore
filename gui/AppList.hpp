#include "TextElement.hpp"
#include "ImageElement.hpp"
#include "AppDetails.hpp"
#include "Sidebar.hpp"
#include "ListElement.hpp"
#include "../libs/get/src/Get.hpp"

class AppList : public ListElement
{
public:
	AppList(Get* get, Sidebar* sidebar);
	bool process(InputEvents* event);
	void render(Element* parent);
	void update();

	Get* get = NULL;
	Sidebar* sidebar = NULL;

	bool touchMode = true;

	// the total number of apps displayed in this list
	int totalCount = 0;
    
    // default number of items per row TODO: save this value as config
    int R = 3;

};

