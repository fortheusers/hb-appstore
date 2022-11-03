#include "MainDisplay.hpp"


#ifndef APP_VERSION
#define APP_VERSION "0.0.0"
#endif

Sidebar::Sidebar()
	: logo(RAMFS "res/icon.png")
	, title("Guest's Game Store", 22)
	, subtitle("v" APP_VERSION " for " PLATFORM, 18)
{
	// a sidebar consists of:
	//		a collection of category labels (TextElements),
	//		and an image (ImageElement) and a logo (TextElement)

	// it also can process input if the cursor goes over it, or a touch

	// there's no back color to the sidebar, as the background is already the right color

	// for every entry in cat names, create a text element
	// elements 0 through TOTAL_CATS are the sidebar texts (for highlighting)
	for (int x = 0; x < TOTAL_CATS; x++)
	{
		category[x].icon = new ImageElement((std::string(RAMFS "res/") + cat_value[x] + ".png").c_str());
		category[x].icon->resize(40, 40);
		category[x].icon->position(30, 150 + x * 70 - 5);
		super::append(category[x].icon);

		category[x].name = new TextElement(cat_names[x], 25);
		category[x].name->position(105, 150 + x * 70);
		super::append(category[x].name);
	}

	// create image in top left
	logo.resize(45, 45);
	logo.position(30, 50);
	super::append(&logo);

	// create title for logo, top left
	title.position(105, 45);
	super::append(&title);

	subtitle.position(105, 75);
	super::append(&subtitle);

	// elasticCounter in this class is used to keep track of which element is being pressed down on in touch mode
	// TODO: elasticCounter belongs to element and should really be renamed (it's for general purpose animations)
	elasticCounter = -1;
}

Sidebar::~Sidebar()
{
	super::removeAll();
	for (int x = 0; x < TOTAL_CATS; x++)
	{
		delete category[x].icon;
		delete category[x].name;
	}
	if (hider)
		delete hider;
	if (hint)
		delete hint;
}

void Sidebar::addHints()
{
	// small indicator to switch to advanced view using L
	hider = new ImageElement(RAMFS "res/button-l-outline.png");
	hider->resize(20, 20);
	hider->position(270, 685);
	super::append(hider);

	hint = new TextElement("Hide", 15);
	hint->position(hider->x + hider->width + 5, hider->y);
	super::append(hint);

	showCurrentCategory = true;
}

bool Sidebar::process(InputEvents* event)
{
	bool ret = false;
	int origHighlighted = highlighted;

	auto mainDisplay = (MainDisplay*)RootDisplay::mainDisplay;

	// process some joycon input events
	if (event->isKeyDown())
	{
		// if highlighted isn't set, don't do anything (applist will set it)
		if (this->highlighted < 0)
			return false;

		// if we got a RIGHT key, send it back to the applist
		if (event->held(RIGHT_BUTTON))
		{
			this->highlighted = -1;
			this->appList->highlighted = 0;
			this->appList->y = 0; // reset scroll TODO: maintain scroll when switching between sidebar and app list
			event->update();
			mainDisplay->playSFX();
			return true;
		}

		// adjust the cursor by 1 for up or down
		this->highlighted += -1 * (event->held(UP_BUTTON)) + (event->held(DOWN_BUTTON));

		// don't let the cursor go out of bounds
		if (this->highlighted < 0) this->highlighted = 0;
		if (this->highlighted >= TOTAL_CATS) this->highlighted = TOTAL_CATS - 1;
	}

#if !defined(_3DS) && !defined(_3DS_MOCK)
	// saw click down, set dragging state
	if (event->isTouchDown())
	{
		this->dragging = true;
		this->highlighted = -1;

		// go through the categories and see if this touch down was in one of them, to show it highlighted
		// TODO: uses similar code from below... really all this sidebar stuff shoould be refactored to use ListElement
		// and every category itself should be a CategoryLabel just like an AppCard consists of images + text
		for (int x = 0; x < TOTAL_CATS; x++)
		{
			int xc = 0, yc = 150 + x * 70 - 15, width = 400 - 260 * (appList->R - 3) - 35, height = 60;
			if (event->touchIn(xc, yc, width, height))
			{
				// touch is over an element of the sidebar, set the elasticCounter
				elasticCounter = x;
				break;
			}
		}

		return true;
	}
#endif

	// detect if a click is on one of the sidebar elements
	// (or we saw the A button be pressed)
	if ((event->isTouchUp() && this->dragging) || (event->isKeyDown() && event->held(A_BUTTON)))
	{
		this->dragging = false;
		int previouslySelected = elasticCounter;
		elasticCounter = -1; // reset highlighted one

		// check if it's one of the text elements
		for (int x = 0; x < TOTAL_CATS; x++)
		{
			int xc = 0, yc = 150 + x * 70 - 15, width = 400 - 260 * (appList->R - 3) - 35, height = 60; // TODO: extract formula into method (same as AppList x value)
			if ((event->touchIn(xc, yc, width, height) && event->isTouchUp()) || (event->held(A_BUTTON) && this->highlighted == x))
			{
				// if it's a touch up, let's make sure this is the same one we touched down on
				if (event->isTouchUp() && previouslySelected >= 0 && x != previouslySelected)
					return true;

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

		return true;
	}

	if (origHighlighted != highlighted) {
		mainDisplay->playSFX();
		ret |= true;
	}

	return ret;
}

void Sidebar::render(Element* parent)
{
#if defined(_3DS) || defined(_3DS_MOCK)
  // no sidebar on 3ds
  return;
#endif
	// draw the light gray bg behind the active category
	CST_Rect dimens = { 0, 0, 400 - 260 * (appList->R - 3) - 35, 60 }; // TODO: extract this to a method too
	dimens.y = 150 + this->curCategory * 70 - 15;					   // TODO: extract formula into method

	auto c = RootDisplay::mainDisplay->backgroundColor;
	CST_Color consoleColor = { (int)(c.r * 255) + 0x25, (int)(c.g * 255) + 0x25, (int)(c.b * 255) + 0x25, 0xff };
	// consoleColor.r = fmax(consoleColor.r, 0xff);
	CST_SetDrawColor(RootDisplay::renderer, consoleColor);

	if (this->showCurrentCategory)
		CST_FillRect(RootDisplay::renderer, &dimens);

	if (appList && appList->touchMode && this->elasticCounter >= 0)
	{
		CST_Rect dimens2 = { 0, 0, 400, 60 };
		dimens2.y = 150 + this->elasticCounter * 70 - 15; // TODO: extract formula into method
		CST_SetDrawBlend(RootDisplay::renderer, true);
		CST_Color highlight = { 0xad, 0xd8, 0xe6, 0x90 };
		CST_SetDrawColor(RootDisplay::renderer, highlight); // TODO: matches the DEEP_HIGHLIGHT color
		CST_FillRect(RootDisplay::renderer, &dimens2);
	}

	// draw the selected category, if one should be highlighted
	if (this->highlighted >= 0)
	{
		int y = 150 + this->highlighted * 70 - 15;
		//        rectangleRGBA(RootDisplay::renderer, 0, y, dimens.w, y + dimens.h, 0xff, 0x00, 0xff, 0xff);

		for (int x = 0; x < 5; x++)
		{
			rectangleRGBA(RootDisplay::renderer, dimens.x + x, y + x, dimens.x + dimens.w - x, y + dimens.h - x, 0x66 - x * 10, 0x7c + x * 20, 0x89 + x * 10, 0xFF);
		}
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
