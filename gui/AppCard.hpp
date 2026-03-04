#ifndef APP_CARD_H
#define APP_CARD_H

#include <memory>
#include "../libs/get/src/Package.hpp"

#include "../libs/chesto/src/RootDisplay.hpp"
#include "../libs/chesto/src/ImageElement.hpp"
#include "../libs/chesto/src/NetImageElement.hpp"
#include "../libs/chesto/src/TextElement.hpp"

using namespace Chesto;

class AppList;

class AppCard : public Element
{
public:
	AppCard(Package& package, AppList* list = nullptr);
	~AppCard();
	void update();
	void displaySubscreen();

	std::shared_ptr<Package> package;
	AppList* list;

	// the number of which package this is in the list
	int index;

	// app icon
	NetImageElement* icon;

private:
	static CST_Color gray, black;

	// version
	TextElement* version;
	// status string
	TextElement* status;
	// app name
	TextElement* appname;
	// author
	TextElement* author;
	// download status icon
	ImageElement* statusicon;
};

#endif