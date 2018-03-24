#include "AppCard.hpp"

AppCard::AppCard(Package* package)
{
	this->package = package;
}

void AppCard::update()
{
	// create the layout of the app card (all relative)
	
	// icon (TODO: look up cached image from load)
	ImageElement* icon = new ImageElement("res/default.png");
	icon->position(this->x + 10, this->y + 10);
	this->elements.push_back(icon);
	
	int size = 13;
	
	SDL_Color gray = { 80, 80, 80, 0xff };
	SDL_Color black = { 0, 0, 0, 0xff };
	
	// version
	TextElement* version = new TextElement(("v. " + package->version).c_str(), size, &gray);
	version->position(this->x + 50, this->y + 170);
	this->elements.push_back(version);

	// status string
	TextElement* status = new TextElement(package->statusString(), size, &gray);
	status->position(this->x + 50, this->y + 185);
	this->elements.push_back(status);
	
	// app name
	TextElement* appname = new TextElement(package->title.c_str(), size+3, &black);
	appname->position(this->x + 255 - appname->textSurface->w, this->y + 165);
	this->elements.push_back(appname);
	
	// author
	TextElement* author = new TextElement(package->author.c_str(), size, &gray);
	author->position(this->x + 255 - author->textSurface->w, this->y + 185);
	this->elements.push_back(author);
	
	// download status icon
	// TODO: this
}

void AppCard::render(Element* parent)
{
	// TODO: don't render this card if it's going to be offscreen anyway according to the parent (AppList)
//	if (((AppList*)parent)->scrollOffset)
	
	// render all the subelements of this card
	super::render(parent);
}
