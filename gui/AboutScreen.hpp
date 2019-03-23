#ifndef ABOUTSCREEN_H_
#define ABOUTSCREEN_H_

#include "libget/src/Get.hpp"
#include "ImageElement.hpp"
#include "ListElement.hpp"
#include "TextElement.hpp"

class AboutScreen : public Element
{
public:
	AboutScreen(Get* get);
	Get* get = NULL;
	void render(Element* parent);

	// button bindings
	void back();
	void removeEmptyFolders();
	void wipeCache();
	void launchFeedback();
};

#endif
