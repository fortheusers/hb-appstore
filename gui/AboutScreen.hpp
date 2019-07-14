#ifndef ABOUTSCREEN_H_
#define ABOUTSCREEN_H_

#include "libget/src/Get.hpp"

#include "chesto/src/ImageElement.hpp"
#include "chesto/src/ListElement.hpp"
#include "chesto/src/TextElement.hpp"

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
