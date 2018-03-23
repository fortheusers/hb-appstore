#include "AppList.hpp"
#include "AppCard.hpp"

AppList::AppList(Get* get)
{
	this->x = 410 + 40;
	
	// the main get instance that contains repo info and stuff
	this->get = get;
	
	this->category = "*";
	
	// update current app listing
	update();
}

bool AppList::process()
{
	
	return true;
}

void AppList::render(Element* parent)
{
	// draw a white background, 870 wide
	SDL_Rect dimens = { 0, 0, 870, 720 };
	dimens.x = this->x;
	
	SDL_FillRect(parent->window_surface, &dimens, SDL_MapRGBA(parent->window_surface->format, 0xFF, 0xFF, 0xFF, 0xFF));
	this->window_surface = parent->window_surface;
	
	super::render(this);
}

void AppList::update()
{
	// remove any old elements
	this->wipeElements();
	
	// total apps we're interested in so far
	int count = 0;
	
	for (int x=0; x<get->packages.size(); x++)
	{
		// if we're on all categories, or this package matches the current category
		if (this->category == "*" || get->packages[x]->category == this->category)
		{
			AppCard* card = new AppCard(get->packages[x]);
			
			// position at proper x, y coordinates (TODO: out of bounds for screen -> don't draw)
			card->position(10 + (x%3)*265, 50 + 250*(x/3));
			card->update();
			
			this->elements.push_back(card);
			
			// we drew an app, so increase the displayed app counter
			count ++;
		}
	}
}
