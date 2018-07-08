#include "AppList.hpp"
#include <SDL2/SDL2_gfxPrimitives.h>

Sidebar::Sidebar()
{
	// a sidebar consists of:
	//		a collection of category labels (TextElements),
	//		and an image (ImageElement) and a logo (TextElement)

	// it also can process input if the cursor goes over it, or a touch

	// there's no back color to the sidebar, as the background is already the right color

	// create image in top left
	ImageElement* logo = new ImageElement("res/icon_small.png");
	logo->resize(35, 35);
	logo->position(50, 50);
	this->elements.push_back(logo);

	// create title for logo, top left
	TextElement* title = new TextElement("Switch appstore", 27);
	title->position(90, 50);
	this->elements.push_back(title);

	// for every entry in cat names, create a text element
	for (int x=0; x<TOTAL_CATS; x++)
	{
		TextElement* category = new TextElement(cat_names[x], 25);
		category->position(50, 150+x*70);
		this->elements.push_back(category);
	}
}

bool Sidebar::process(InputEvents* event)
{
	// process some joycon input events
	if (event->isKeyDown())
	{
		// if highlighted isn't set, don't do anything (applist will set it)
		if (this->highlighted < 0)
			return false;

		// if we got a RIGHT key, send it back to the applist
		if (event->held(SDLK_RIGHT))
		{
			this->highlighted = -1;
			this->appList->highlighted = 0;
			this->appList->y = 0;		// reset scroll TODO: maintain scroll when switching between sidebar and app list
			event->update();
			return false;
		}

		// adjust the cursor by 1 for up or down
		this->highlighted += -1*(event->held(UP_BUTTON)) + (event->held(DOWN_BUTTON));

		// don't let the cursor go out of bounds
		if (this->highlighted < 0) this->highlighted = 0;
		if (this->highlighted >= TOTAL_CATS) this->highlighted = TOTAL_CATS-1;
	}

	// saw click down, set dragging state
	if (event->isTouchDown())
	{
		this->dragging = true;
		this->highlighted = -1;
		return false;
	}

	// detect if a click is on one of the sidebar elements
	// (or we saw the A button be pressed)
	if ((event->isKeyUp() && this->dragging) ||
		(event->isKeyDown() && event->held(A_BUTTON)))
	{
		this->dragging = false;

		// check if it's one of the text elements
		for (int x=0; x<TOTAL_CATS; x++)
		{
			int xc = 0, yc = 150+x*70 - 15, width = 400, height = 60;		// TODO: extract formula into method (same as below)
			if ((event->touchIn(xc, width, yc, height) ||
				(event->held(A_BUTTON)) && this->highlighted == x))
			{
				// saw touchup on a category, adjust active category
				this->curCategory = x;

				// since we updated the active category, we need to update the app listing
				if (this->appList != NULL)
				{
					this->appList->y = 0;
					this->appList->update();
				}
			}
		}
	}

	return false;
}

void Sidebar::render(Element* parent)
{
	// draw the light gray bg behind the active category
	SDL_Rect dimens = { 0, 0, 400, 60 };
	dimens.y = 150+this->curCategory*70 - 15;		// TODO: extract formula into method

	SDL_SetRenderDrawColor(parent->renderer, 0x67, 0x6a, 0x6d, 0xFF);
	SDL_RenderFillRect(parent->renderer, &dimens);

	// draw the selected category, if one should be highlighted
	if (this->highlighted >= 0)
	{
		int y = 150+this->highlighted*70 - 15;
		rectangleRGBA(parent->renderer, 0, y, dimens.w, y + dimens.h, 0xff, 0x00, 0xff, 0xff);
	}

	// render subelements
	super::render(parent);
}

std::string Sidebar::currentCatName()
{
	if (this->curCategory >= 0 && this->curCategory < TOTAL_CATS)
		return std::string(this->cat_names[this->curCategory]);

	return std::string("?");
}

std::string Sidebar::currentCatValue()
{
	if (this->curCategory >= 0 && this->curCategory < TOTAL_CATS)
		return std::string(this->cat_value[this->curCategory]);

	return std::string("?");
}
