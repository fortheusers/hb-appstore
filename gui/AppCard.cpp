#include "AppCard.hpp"
#include "MainDisplay.hpp"

AppCard::AppCard(Package* package)
{
	this->package = package;

	// fixed width+height of one app card
	this->width = 256;
	this->height = 195;
    
    this->touchable = true;
}

void AppCard::update()
{
	// create the layout of the app card (all relative)

	// icon, and look up cached image to load
	ImageElement* icon = new ImageElement((ImageCache::cache_path + this->package->pkg_name + "/icon.png").c_str());
	icon->position(this->x, this->y);
	icon->resize(256, 150);
	this->elements.push_back(icon);

	int size = 13;

	SDL_Color gray = { 80, 80, 80, 0xff };
	SDL_Color black = { 0, 0, 0, 0xff };

	// version
	TextElement* version = new TextElement(("v. " + package->version).c_str(), size, &gray);
	version->position(this->x + 40, this->y + 160);
	this->elements.push_back(version);

	// status string
	TextElement* status = new TextElement(package->statusString(), size, &gray);
	status->position(this->x + 40, this->y + 175);
	this->elements.push_back(status);

	// app name
	int w, h;
	TextElement* appname = new TextElement(package->title.c_str(), size+3, &black);
	SDL_QueryTexture(appname->textSurface, NULL, NULL, &w, &h);
	appname->position(this->x + 245 - w, this->y + 155);
	this->elements.push_back(appname);

	// author
	TextElement* author = new TextElement(package->author.c_str(), size, &gray);
	SDL_QueryTexture(author->textSurface, NULL, NULL, &w, &h);
	author->position(this->x + 245 - w, this->y + 175);
	this->elements.push_back(author);

	// download status icon
	ImageElement* statusicon = new ImageElement(("res/" + std::string(package->statusString()) + ".png").c_str());
	statusicon->position(this->x + 4, this->y + 160);
	statusicon->resize(30, 30);
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

bool AppCard::process(InputEvents* event)
{
	if (this->parent == NULL)
		return false;
    
    this->xOff = this->parent->x;
    this->yOff = this->parent->y;
    
    bool ret = false;
    
	if (this->onTouchDown(event))
	{
		// mouse pushed down, set variable
		this->dragging = true;
		this->lastMouseY = event->yPos;
        this->lastMouseX = event->xPos;
	}
    else if (event->isTouchDrag())
    {
        // we've dragged out of the icon, invalidate the click by invoking onTouchUp early
        // check if we haven't drifted too far from the starting variable (treshold: 40)
        if (this->dragging && (abs(event->yPos - this->lastMouseY) >= 40 || abs(event->xPos - this->lastMouseX) >= 40))
            this->elasticCounter = NO_HIGHLIGHT;
    }
	// mouse is up, or A is pressed
	// (if it's A that's being pressed, make sure that our index matches the highlighted value)
	else if (this->onTouchUp(event))
	{
        // ensure we were dragging first (originally checked the treshold above here, but now that actively invalidates it)
		if (this->dragging)
		{
			// check that this click is in the right coordinates for this square
			// and that a subscreen isn't already being shown
			// (and also let the A press through)
			if (((event->touchIn(this->xOff + this->x, this->yOff + this->y, this->width, this->height)) ||
				event->held(A_BUTTON)) &&
                !MainDisplay::subscreen)
			{
				// received a click on this app, add a subscreen under the parent
				// (parent of AppCard should be AppList)
                AppList* appList = ((AppList*)this->parent);
                MainDisplay::subscreen = new AppDetails(this->package, appList);
				if (!appList->touchMode)
                    ((AppDetails*)MainDisplay::subscreen)->highlighted = 0;		// show cursor if we're not in touch mode
                ret |= true;
			}
		}

		// release mouse
		this->dragging = false;
	}

	return ret;
}
