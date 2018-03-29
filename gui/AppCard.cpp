#include "AppCard.hpp"
#include "AppList.hpp"
#include "ImageCache.hpp"

AppCard::AppCard(Package* package)
{
	this->package = package;
	
	// fixed width+height of one app card
	this->width = 265;
	this->height = 205;
}

void AppCard::update()
{
	// create the layout of the app card (all relative)
	
	// icon, and look up cached image to load
	ImageElement* icon = new ImageElement((ImageCache::cache_path + this->package->pkg_name + "/icon.png").c_str());
	icon->position(this->x + 10, this->y + 10);
	icon->resize(256, 150);
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
	ImageElement* statusicon = new ImageElement(("res/" + std::string(package->statusString()) + ".png").c_str());
	statusicon->position(this->x + 14, this->y + 170);
	this->elements.push_back(statusicon);
}

void AppCard::render(Element* parent)
{
	// grab and store the parent while we have it, and if we need it
	if (this->parent == NULL)
		this->parent = parent;
	
	// TODO: don't render this card if it's going to be offscreen anyway according to the parent (AppList)
//	if (((AppList*)parent)->scrollOffset)
	
	// render all the subelements of this card
	super::render(parent);
}

bool AppCard::process(SDL_Event* event)
{
	if (this->parent == NULL)
		return false;
	
	if (event->type == SDL_MOUSEBUTTONDOWN)
	{
		// mouse pushed down, set variable
		this->dragging = true;
		this->lastMouseY = event->motion.y;
	}
	// mouse is up, or A is pressed
	// (if it's A that's being pressed, make sure that our index matches the highlighted value)
	else if (event->type == SDL_MOUSEBUTTONUP ||
			(event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_a && ((AppList*)parent)->highlighted == this->index))
	{
		// check if we haven't drifted too far from the starting variable (treshold: 40)
		// (or it's a key event, just let it through)
		if ((this->dragging && abs(event->motion.y - this->lastMouseY) < 40 && this->parent)
			|| event->type == SDL_KEYDOWN)
		{
			// check that this click is in the right coordinates for this square
			// and that a subscreen isn't already being shown
			// (and also let the A press through)
			if (((event->motion.x >= this->parent->x + this->x &&
				event->motion.x <= this->parent->x + this->x + this->width &&
				event->motion.y >= this->parent->y + this->y &&
				event->motion.y <= this->parent->y + this->y + this->height) ||
				event->key.keysym.sym == SDLK_a) &&
				!((AppList*)this->parent)->subscreen)
			{
				// received a click on this app, add a subscreen under the parent
				// (parent of AppCard should be AppList)
				AppPopup* popup = new AppPopup(this->package);
				AppList* appList = ((AppList*)this->parent);
				if (!appList->touchMode)
					popup->highlighted = 0;		// show cursor if we're not in touch mode
				this->parent->elements.push_back(popup);
				
				// set the subscreen variable too, to acces it easier
				appList->subscreen = popup;
				AppPopup::frontmostPopup = popup;
			}
		}
		
		// release mouse
		this->dragging = false;
	}
	
	return false;
}
