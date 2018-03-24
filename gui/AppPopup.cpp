#include "AppList.hpp"
#include "ProgressBar.hpp"
#include "../libs/get/src/Get.hpp"

AppPopup::AppPopup(Package* package)
{
	this->package = package;
	
	ImageElement* shade = new ImageElement("res/shade.png");
	this->elements.push_back(shade);
	
	ImageElement* popup = new ImageElement("res/popup.png");
	popup->position(469, 109);
	this->elements.push_back(popup);
	
	// progress bar, element 2
	ProgressBar* pbar = new ProgressBar();
	pbar->position(580, 450);
	pbar->color = 0xff0000ff;
	pbar->width = 500;
	this->elements.push_back(pbar);
	
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
	
	// download/update/remove button (3)
	TextElement* download = new TextElement(action, 25, &red);
	download->position(608, 480);
	this->elements.push_back(download);
	
	// close button (4)
	TextElement* close = new TextElement("Close", 25, &red);
	close->position(993, 480);
	this->elements.push_back(close);
	
	TextElement* title = new TextElement(package->title.c_str(), 30, &black);
	title->position(480, 116);
	this->elements.push_back(title);
	
	// TODO: replace with actual screen shot from cache
	ImageElement* screen = new ImageElement("res/noscreen.png");
	screen->position(469, 160);
	this->elements.push_back(screen);
	
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
	// don't process any keystrokes if an operation is in progress
	if (operating)
		return false;
	
	// we need to detect if they hit download/update/remove or close
	// (this is not a great way to do this)
	if (event->type == SDL_MOUSEBUTTONUP)
	{
		if (this->parent == NULL)
			return false;
		
		int x = 570, y = 465;
		int x2 = 950;
		int w = 130, h = 55;
		
		int mx = event->motion.x;
		int my = event->motion.y;
		
		if (mx >= x &&
			mx <= x + w &&
			my >= y &&
			my <= y + h)
		{
			this->operating = true;
			
			// setup progress bar callback
			// TODO: this
//			this->package->progresssCallback =
			
			// install or remove this package based on the package status
			if (this->package->status == INSTALLED)
				((AppList*)this->parent)->get->remove(this->package);
			else
				((AppList*)this->parent)->get->install(this->package);
			
			// refresh the screen
			this->wipeElements();
			((AppList*)this->parent)->subscreen = NULL;
			
			((AppList*)this->parent)->update();
			
			this->operating = false;
		}
		
		if (mx >= x2 &&
			mx <= x2 + w &&
			my >= y &&
			my <= y + h)
		{
			// remove elements on this pop up
			this->wipeElements();
			
			// our parent should also be AppList, tell it that the subscreen is dismissed
			if (this->parent)
			{
				// refresh the screen
				((AppList*)this->parent)->subscreen = NULL;
				((AppList*)this->parent)->update();
			}
		}
	}
	
	return false;
}

void AppPopup::render(Element* parent)
{
	if (this->window_surface == NULL)
		this->window_surface = parent->window_surface;
	
	if (this->parent == NULL)
		this->parent = parent;
	
	for (int x=0; x<this->elements.size(); x++)
	{
		// go through every subelement and run render
		// (use "this" instead of "parent" to be absolute)
		this->elements[x]->render(this);
	}
}
