#include "AppList.hpp"
#include "AppCard.hpp"
#include "Keyboard.hpp"
#include <SDL2/SDL2_gfxPrimitives.h>

AppList::AppList(Get* get, Sidebar* sidebar)
{
    this->x = 400 - 260*(R-3);

	// the offset of how far along scroll'd we are
	this->y = 0;

	// the main get instance that contains repo info and stuff
	this->get = get;

	// the sidebar, which will store the currently selected category info
	this->sidebar = sidebar;

	// update current app listing
	update();
}

bool AppList::process(InputEvents* event)
{
    bool ret = false;
    
    if (event->held(B_BUTTON))
    {
        R = (R==3)? 4 : 3;
        this->x = 400 - 260*(R-3);
        update();
        return true;
    }
    
    // process some joycon input events
    if (event->isKeyDown())
    {
        if (event->held(A_BUTTON | B_BUTTON | UP_BUTTON | DOWN_BUTTON | LEFT_BUTTON | RIGHT_BUTTON))
        {
            // if we were in touch mode, draw the cursor in the applist
            // and reset our position
            if (this->touchMode)
            {
                this->touchMode = false;
                this->highlighted = 0;
                this->y = 0;		// reset scroll TODO: maintain scroll when switching back from touch mode
                return true;
            }

            // touchmode is false, but our highlight value is negative
            // (do nothing, let sidebar update our highlight value)
            if (this->highlighted < 0) return false;
            
            // look up whatever is currently chosen as the highlighted position
            // and remove its highlight
            if (this->elements[this->highlighted])
                this->elements[this->highlighted]->elasticCounter = NO_HIGHLIGHT;

            // if we got a LEFT key while on the left most edge already, transfer to categories
            if (this->highlighted%R==0 && event->held(LEFT_BUTTON))
            {
                this->highlighted = -1;
                this->sidebar->highlighted = 0;
                return true;
            }

            // similarly, prevent a RIGHT from wrapping to the next line
            if (this->highlighted%R==2 && event->held(RIGHT_BUTTON)) return false;

            // adjust the cursor by 1 for left or right
            this->highlighted += -1*(event->held(LEFT_BUTTON)) + (event->held(RIGHT_BUTTON));

            // adjust it by R for up and down
            this->highlighted += -1*R*(event->held(UP_BUTTON)) + R*(event->held(DOWN_BUTTON));

            // don't let the cursor go out of bounds
            if (this->highlighted < 0) this->highlighted = 0;
            if (this->highlighted >= this->totalCount) this->highlighted = this->totalCount-1;

            // if our highlighted position is large enough, force scroll the screen so that our cursor stays on screen
            // TODO: make it so that the cursor can go to the top of the screen
            if (this->highlighted >= R*2)
                this->y = -1*((this->highlighted-R*2)/R)*210 - 60;
            else
                this->y = 0;		// at the top of the screen

            if (this->elements[this->highlighted])
                this->elements[this->highlighted]->elasticCounter = HIGHLIGHT;
        }
    }
    if (event->isTouchDown())
    {
        // got a touch, so let's enter touchmode
        this->highlighted = -1;
        this->touchMode = true;
    }

    // perform inertia scrolling for this element
    ret |= this->handleInertiaScroll(event);
    
	ret |= super::process(event);

	return ret;
}

void AppList::render(Element* parent)
{
	if (this->parent == NULL)
		this->parent = parent;

	// draw a white background, 870 wide
	SDL_Rect dimens = { 0, 0, 920 + 260*(R-3), 720 };
	dimens.x = this->x - 35;

	SDL_SetRenderDrawColor(parent->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(parent->renderer, &dimens);
	this->renderer = parent->renderer;

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

	// the current category value from the sidebar
	std::string curCategoryValue = this->sidebar->currentCatValue();

	// all packages TODO: move some of this filtering logic into main get library
	std::vector<Package*> packages = get->packages;

	// if it's a search, do a search query through get rather than using all packages
	if (curCategoryValue == "_search")
		packages = get->search(this->sidebar->searchQuery);

	// update
	for (int x=0; x<packages.size(); x++)
		if (packages[x]->status == UPDATE)
			sorted.push_back(packages[x]);

	// installed
	for (int x=0; x<packages.size(); x++)
		if (packages[x]->status == INSTALLED)
			sorted.push_back(packages[x]);

	// get
	for (int x=0; x<packages.size(); x++)
		if (packages[x]->status == GET)
			sorted.push_back(packages[x]);

	// total apps we're interested in so far
	int count = 0;
    
	for (int x=0; x<sorted.size(); x++)
	{
		// if we're on all categories, or this package matches the current category (or it's a search (prefiltered))
		if (curCategoryValue == "*" || curCategoryValue == sorted[x]->category || curCategoryValue == "_search")
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
		card->position(25 + (x%R)*265, 145 + 210*(x/R));		// TODO: extract formula into method (see above)
		card->update();
	}

	// the title of this category (from the sidebar)
	SDL_Color black = { 0, 0, 0, 0xff };
	TextElement* category;

	// if it's a search, add a keyboard
	if (curCategoryValue == "_search")
	{
		Keyboard* keyboard = new Keyboard(this);
		this->elements.push_back(keyboard);

		category = new TextElement((std::string("Search: \"") + this->sidebar->searchQuery + "\"").c_str(), 28, &black);
	}
	else
	{
		category = new TextElement(this->sidebar->currentCatName().c_str(), 28, &black);
	}

	category->position(20, 90);
	this->elements.push_back(category);
}
