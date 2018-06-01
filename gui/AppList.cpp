#include "AppList.hpp"
#include "AppCard.hpp"
#include <SDL2/SDL2_gfxPrimitives.h>

AppList::AppList(Get* get, Sidebar* sidebar)
{
	this->x = 400;

	// the offset of how far along scroll'd we are
	this->y = 0;

	// the main get instance that contains repo info and stuff
	this->get = get;

	// the sidebar, which will store the currently selected category info
	this->sidebar = sidebar;

	// update current app listing
	update();
}

bool AppList::process(SDL_Event* event)
{
	// only process any events for AppList if there's no subscreen
	if (this->subscreen == NULL)
	{
		// process some joycon input events
		if (event->type == SDL_KEYUP)
		{
			if (event->key.keysym.sym == SDLK_UP || event->key.keysym.sym == SDLK_DOWN ||
				event->key.keysym.sym == SDLK_LEFT || event->key.keysym.sym == SDLK_RIGHT ||
				event->key.keysym.sym == SDLK_a || event->key.keysym.sym == SDLK_b)
			{
				// if we were in touch mode, draw the cursor in the applist
				// and reset our position
				if (this->touchMode)
				{
					this->touchMode = false;
					this->highlighted = 0;
					this->y = 0;		// reset scroll TODO: maintain scroll when switching back from touch mode
					return false;
				}

				// touchmode is false, but our highlight value is negative
				// (do nothing, let sidebar update our highlight value)
				if (this->highlighted < 0) return false;

				// if we got a LEFT key while on the left most edge already, transfer to categories
				if (this->highlighted%3==0 && event->key.keysym.sym == SDLK_LEFT)
				{
					this->highlighted = -1;
					this->sidebar->highlighted = 0;
					return false;
				}

				// similarly, prevent a RIGHT from wrapping to the next line
				if (this->highlighted%3==2 && event->key.keysym.sym == SDLK_RIGHT) return false;

				// adjust the cursor by 1 for left or right
				this->highlighted += -1*(event->key.keysym.sym == SDLK_LEFT) + (event->key.keysym.sym == SDLK_RIGHT);

				// adjust it by 3 for up and down
				this->highlighted += -3*(event->key.keysym.sym == SDLK_UP) + 3*(event->key.keysym.sym == SDLK_DOWN);

				// don't let the cursor go out of bounds
				if (this->highlighted < 0) this->highlighted = 0;
				if (this->highlighted >= this->totalCount) this->highlighted = this->totalCount-1;

				// if our highlighted position is large enough, force scroll the screen so that our cursor stays on screen
				// TODO: make it so that the cursor can go to the top of the screen
				if (this->highlighted >= 6)
					this->y = -1*((this->highlighted-6)/3)*210 - 60;
				else
					this->y = 0;		// at the top of the screen

			}
		}

		if (event->type == SDL_MOUSEBUTTONDOWN)
		{
			// got a touch, so let's enter touchmode
			this->highlighted = -1;
			this->touchMode = true;

			// make sure that the mouse down's X coordinate is over the app list (not sidebar)
			if (event->motion.x < this->x)
				return false;

			// saw mouse down so set it in our element object
			this->dragging = true;
			this->lastMouseY = event->motion.y;
		}
		// drag event for scrolling up or down
		else if (event->type == SDL_MOUSEMOTION)
		{
			if (this->dragging)
			{
				int distance = event->motion.y - this->lastMouseY;
				this->y += distance;
				this->lastMouseY = event->motion.y;

				// use the last distance as the rubber band value
				this->elasticCounter = distance;
			}
		}
		else if (event->type == SDL_MOUSEBUTTONUP)
		{
			// mouse up, no more mouse down (TODO: fire selected event here)
			this->dragging = false;

			// if the scroll offset is less than the total number of apps
			// (put on the mouse up to make it "snap" when going out of bounds)
			// TODO: account for max number of apps too (prevent scrolling down forever)
			if (this->y > 0)
				this->y = 0;
		}

		// if mouse is up, and there's some elastic counter left, burn out remaining elastic value
		if (!this->dragging && this->elasticCounter != 0)
		{
			this->y += this->elasticCounter;

			int positivity = this->elasticCounter/abs(this->elasticCounter);
			this->elasticCounter += 10 * (-1 * positivity);

			// when the oval and the elastic counter don't match in positivity, reset it to 0
			if (this->elasticCounter != 0 && this->elasticCounter/abs(this->elasticCounter) != positivity)
				this->elasticCounter = 0;

			// TODO: same problem as above todo, also extract into method?
			if (this->y > 0)
				this->y = 0;
		}
	}

	super::process(event);

	return true;
}

void AppList::render(Element* parent)
{
	if (this->parent == NULL)
		this->parent = parent;

	// draw a white background, 870 wide
	SDL_Rect dimens = { 0, 0, 920, 720 };
	dimens.x = this->x - 35;

	SDL_SetRenderDrawColor(parent->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(parent->renderer, &dimens);
	this->renderer = parent->renderer;

	// draw the cursor at the highlighted position, if appropriate
	if (this->highlighted >= 0)
	{
		int x = this->x + 15 + (this->highlighted%3)*265;		// TODO: extract into formula method
		int y = this->y + 135 + 210*(this->highlighted/3);

		rectangleRGBA(parent->renderer, x, y, x + 265, y + 205, 0xff, 0x00, 0xff, 0xff);
	}

	super::render(this);
}

void AppList::update()
{
	// remove any old elements
	this->wipeElements();

	// quickly create a vector of "sorted" apps
	// (they must be sorted by UPDATE -> INSTALLED -> GET)
	// TODO: sort this a better way, and also don't use 3 distinct for loops
	std::vector<Package*> sorted;

	// update
	for (int x=0; x<get->packages.size(); x++)
		if (get->packages[x]->status == UPDATE)
			sorted.push_back(get->packages[x]);

	// installed
	for (int x=0; x<get->packages.size(); x++)
		if (get->packages[x]->status == INSTALLED)
			sorted.push_back(get->packages[x]);

	// get
	for (int x=0; x<get->packages.size(); x++)
		if (get->packages[x]->status == GET)
			sorted.push_back(get->packages[x]);

	// total apps we're interested in so far
	int count = 0;

	// the current category value from the sidebar
	std::string curCategoryValue = this->sidebar->currentCatValue();

	for (int x=0; x<sorted.size(); x++)
	{
		// if we're on all categories, or this package matches the current category
		if (curCategoryValue == "*" || curCategoryValue == sorted[x]->category)
		{
			AppCard* card = new AppCard(sorted[x]);
			card->index = count;

			this->elements.push_back(card);

			// we drew an app, so increase the displayed app counter
			count ++;
		}
	}

	this->totalCount = count;

	// position the filtered app card list
	for (int x=0; x<this->elements.size(); x++)
	{
		// every element after the first should be an app card (we just added them)
		AppCard* card = (AppCard*) elements[x];

		// position at proper x, y coordinates
		card->position(10 + (x%3)*265, 130 + 210*(x/3));		// TODO: extract formula into method (see above)
		card->update();
	}

	// the title of this category (from the sidebar)
	SDL_Color black = { 0, 0, 0, 0xff };
	TextElement* category = new TextElement(this->sidebar->currentCatName().c_str(), 28, &black);
	category->position(20, 90);
	this->elements.push_back(category);
}
