#pragma once

#include "libget/src/Package.hpp"

#include "chesto/src/RootDisplay.hpp"
#include "chesto/src/ImageElement.hpp"
#include "chesto/src/NetImageElement.hpp"
#include "chesto/src/TextElement.hpp"

#include "AppList.hpp"

class AppCard : public Element
{
public:
	AppCard(Package* package, AppList* list = nullptr);
	void update();
	bool process(InputEvents* event);
	void render(Element* parent);
	void displaySubscreen();
	void handleIconLoad();

	Package* package;
	AppList* list;
	NetImageElement* icon;
	bool iconFetch = false;

	// the number of which package this is in the list
	int index;
};
