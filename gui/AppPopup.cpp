#include "AppPopup.hpp"
#include "ProgressBar.hpp"
#include "../libs/get/src/Get.hpp"

AppPopup::AppPopup(Package* package)
{
	ImageElement* shade = new ImageElement("res/shade.png");
	this->elements.push_back(shade);
	
	ImageElement* popup = new ImageElement("res/popup.png");
	popup->position(469, 109);
	this->elements.push_back(popup);
	
	SDL_Color red = {0xFF, 0x00, 0x00, 0xff};
	SDL_Color gray = {0x50, 0x50, 0x50, 0xff};
	SDL_Color black = {0x00, 0x00, 0x00, 0xff};

	
	const char* action;
	switch (package->status)
	{
		case GET:
			action = "Download";
			break;
		case UPDATE:
			action = "Update";
			break;
		case INSTALLED:
			action = "Remove";
			break;
		default:
			action = "?";
	}
	
	TextElement* title = new TextElement(package->title.c_str(), 30, &black);
	title->position(480, 116);
	this->elements.push_back(title);
	
	// TODO: replace with actual screen shot from cache
	ImageElement* screen = new ImageElement("res/noscreen.png");
	screen->position(469, 160);
	this->elements.push_back(screen);
	
//	ProgressBar* pbar = new ProgressBar();
//	pbar->position(590, 500);
//	pbar->color = 0xff0000ff;
//	pbar->width = 500;
//	this->elements.push_back(pbar);
	
	TextElement* download = new TextElement(action, 25, &red);
	download->position(608, 480);
	this->elements.push_back(download);
	
	TextElement* close = new TextElement("Close", 25, &red);
	close->position(993, 480);
	this->elements.push_back(close);
	
	
	// app+package+author info:
	TextElement* title2 = new TextElement(package->title.c_str(), 20, &black);
	title2->position(550, 345);
	this->elements.push_back(title2);
	
	TextElement* author = new TextElement(("- " + package->author).c_str(), 20, &gray);
	author->position(550 + title2->textSurface->w + 5, 345);
	this->elements.push_back(author);
	
	TextElement* subtitle = new TextElement(package->short_desc.c_str(), 20, &gray);
	subtitle->position(550, 370);
	this->elements.push_back(subtitle);
	
	TextElement* version = new TextElement(package->version.c_str(), 17, &gray);
	version->position(550, 395);
	this->elements.push_back(version);
	
	
}

bool AppPopup::process(SDL_Event* event)
{
	// we need to detect if they hit download/update/remove or close
	// (this is not a great way to do this)
	if (event->type == SDL_MOUSEBUTTONUP)
	{
		
	}
	
	return false;
}

void AppPopup::render(Element* parent)
{
	if (this->window_surface == NULL)
		this->window_surface = parent->window_surface;
	
	for (int x=0; x<this->elements.size(); x++)
	{
		// go through every subelement and run render
		// (use "this" instead of "parent" to be absolute)
		this->elements[x]->render(this);
	}
}
