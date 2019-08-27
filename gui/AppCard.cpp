#include "AppCard.hpp"
#include "MainDisplay.hpp"

AppCard::AppCard(Package* package, AppList* list)
{
	this->list = list;
	this->package = package;

	// fixed width+height of one app card
	this->width = 256;
	this->height = ICON_SIZE + 45;

	this->touchable = true;

	// connect the action to the callback for this element, to be invoked when the touch event fires
	this->action = std::bind(&AppCard::displaySubscreen, this);
}

void AppCard::update()
{
	// create the layout of the app card (all relative)

	// If a parent list was passed, the icon will be loaded when
	// its close to being shown on the screen
	iconFetch = !list;
	icon = new NetImageElement(package->getIconUrl().c_str(), []{
			return new ImageElement(ROMFS "res/default.png");
		},
		iconFetch
	);
	icon->position(this->x, this->y);
	icon->resize(256, ICON_SIZE);

	this->elements.push_back(icon);

	int size = 13;

	SDL_Color gray = { 80, 80, 80, 0xff };
	SDL_Color black = { 0, 0, 0, 0xff };

	// version
	TextElement* version = new TextElement(("v. " + package->version).c_str(), size, &gray);
	version->position(this->x + 40, this->y + icon->height + 10);
	this->elements.push_back(version);

	// status string
	TextElement* status = new TextElement(package->statusString(), size, &gray);
	status->position(this->x + 40, this->y + icon->height + 25);
	this->elements.push_back(status);

	// app name
	int w, h;
	TextElement* appname = new TextElement(package->title.c_str(), size + 3, &black);
	appname->getTextureSize(&w, &h);
	appname->position(this->x + 245 - w, this->y + icon->height + 5);
	this->elements.push_back(appname);

	// author
	TextElement* author = new TextElement(package->author.c_str(), size, &gray);
	author->getTextureSize(&w, &h);
	author->position(this->x + 245 - w, this->y + icon->height + 25);
	this->elements.push_back(author);

	// download status icon
	ImageElement* statusicon = new ImageElement((ROMFS "res/" + std::string(package->statusString()) + ".png").c_str());
	statusicon->position(this->x + 4, this->y + icon->height + 10);
	statusicon->resize(30, 30);
	this->elements.push_back(statusicon);
}

// Trigger the icon download (if the icon wasn't already cached)
// when the icon is near the visible part of the screen
void AppCard::handleIconLoad()
{
	if (iconFetch)
		return;

	int twoCardsHeight = (this->height + 15) * 2;

	if ((list->y + this->y + this->height) < -twoCardsHeight)
		return;
	if ((list->y + this->y) > (SCREEN_HEIGHT + twoCardsHeight))
		return;

	// the icon is either visible or ofscreen within 2 rows,
	// so the download can be started
	icon->fetch();

	iconFetch = true;
}

void AppCard::render(Element* parent)
{
	this->xOff = parent->x;
	this->yOff = parent->y;

	// TODO: don't render this card if it's going to be offscreen anyway according to the parent (AppList)
	//	if (((AppList*)parent)->scrollOffset)

	// render all the subelements of this card
	super::render(parent);
}

void AppCard::displaySubscreen()
{
	if (!list)
		return;

	// received a click on this app, add a subscreen under the parent
	AppDetails *appDetails = new AppDetails(this->package, list);

	if (!list->touchMode)
		appDetails->highlighted = 0; // show cursor if we're not in touch mode

	MainDisplay::subscreen = appDetails;
}

bool AppCard::process(InputEvents* event)
{
	if (list)
	{
		handleIconLoad();

		this->xOff = this->list->x;
		this->yOff = this->list->y;
	}

	return super::process(event);
}
