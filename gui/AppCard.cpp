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
	
	int size = 17;
	
	SDL_Color gray = { 40, 40, 40, 0xff };
	SDL_Color black = { 0, 0, 0, 0xff };
	
	// version
	TextElement* version = new TextElement(package->version.c_str(), size, &gray);
	version->position(this->x + 30, this->y + 175);
	this->elements.push_back(version);
	
	// status string
	TextElement* status = new TextElement(package->statusString(), size, &gray);
	status->position(this->x + 30, this->y + 190);
	this->elements.push_back(status);
	
	// app name
	TextElement* appname = new TextElement(package->title.c_str(), size, &black);
	appname->position(this->x + 200, this->y + 175);
	this->elements.push_back(appname);
	
	// author
	TextElement* author = new TextElement(package->author.c_str(), size, &gray);
	author->position(this->x + 200, this->y + 190);
	this->elements.push_back(author);
	
	// download status icon
	// TODO: this
}
