#include "../libs/chesto/src/Constraint.hpp"
#include "MainDisplay.hpp"

#ifndef APP_VERSION
#define APP_VERSION "0.0.0"
#endif

Sidebar::Sidebar()
	: ListElement()
{
	// a sidebar consists of:
	//		a collection of category labels (TextElements),
	//		and an image (ImageElement) and a logo (TextElement)

	// it also can process input if the cursor goes over it, or a touch

	// for every entry in cat names, create a text element
	// elements 0 through TOTAL_CATS are the sidebar texts (for highlighting)

	auto effectiveScale = getEffectiveScale();
	for (int x = 0; x < TOTAL_CATS; x++)
	{
		category[x].icon = createNode<ImageElement>((std::string(RAMFS "res/") + cat_value[x] + ".png").c_str());
		category[x].icon->resize(40 * effectiveScale, 40 * effectiveScale);
		category[x].icon->position(30 * effectiveScale, 150 * effectiveScale + x * 70 * effectiveScale - 5 * effectiveScale);

		category[x].name = createNode<TextElement>(i18n(cat_names[x]), 25);
		category[x].name->position(105 * effectiveScale, 150 * effectiveScale + x * 70 * effectiveScale);
	}

	// if we're a dev build, rotate the icon upside down
	logo = createNode<ImageElement>(LOGO_PATH);
#if defined(DEBUG_BUILD) && !defined(WII_MOCK)
	logo->angle = 180;
#endif

	// create image in top left
	logo->resize(45 * effectiveScale, 45 * effectiveScale);
	logo->position(30 * effectiveScale, 50 * effectiveScale);

	// create title for logo, top left
#if defined(DEBUG_BUILD) && !defined(WII_MOCK)
	title = createNode<TextElement>("hb-appstore Dev Build", 22);
	subtitle = createNode<TextElement>("v" APP_VERSION " (" __DATE__ ")", 18);
#else
	title = createNode<TextElement>("Homebrew App Store", 22);
	subtitle = createNode<TextElement>("v" APP_VERSION " for " PLATFORM, 18);
#endif

	title->position(105 * effectiveScale, 45 * effectiveScale);
	subtitle->position(105 * effectiveScale, 75 * effectiveScale);

#if defined(USE_OSC_BRANDING)
	// make the icon larger
	logo->setScaleMode(SCALE_PROPORTIONAL_NO_BG);
	logo->resize(85, 85);
	logo->position(10, 20);

	title->setText("HB App Store v" APP_VERSION);
	title->update();
	subtitle->setText("Pwrd by OSCWii.org");
	subtitle->setSize(22);
	subtitle->update();

	title->position(110, 35);
	subtitle->position(110, 65);
#endif

	// currentSelection in this class is used to keep track of which element is being pressed down on in touch mode
	// TODO: currentSelection belongs to element and should really be renamed (it's for general purpose animations)
	currentSelection = -1;

	if (isEarthDay())
	{
		// easter egg for earth day https://www.earthday.org
		title->setText(i18n("listing.earthday"));
		title->update();

		// draw a an icon over the logo
		logo->hide();
		ImageElement* earth = createNode<ImageElement>(RAMFS "res/earth.png");
		earth->resize(60 * effectiveScale, 60 * effectiveScale);
		earth->position(23 * effectiveScale, 40 * effectiveScale);
	}
}

Sidebar::~Sidebar()
{
	super::removeAll();
}

void Sidebar::addHints()
{
	auto effectiveScale = getEffectiveScale();

	if (hider == nullptr)
	{
		// small indicator to switch to advanced view using L
		hider = createNode<ImageElement>(Button::getControllerButtonImageForPlatform(L_BUTTON, false, false));
		hider->resize(20 * effectiveScale, 20 * effectiveScale);
	}

	if (hint == nullptr)
	{
		hint = createNode<TextElement>(i18n("sidebar.hide"), 15);
	}

	hider->constrain(ALIGN_RIGHT | ALIGN_BOTTOM, 10);
	hint->position(hider->x + hider->width + 5 * effectiveScale, hider->y);
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
	if (event->isTouchDown() || event->isTouchDrag())
	{
		this->dragging = true;
		this->highlighted = -1;
		this->elasticCounter = -1;
		this->currentSelection = -1;

		float effectiveScale = getEffectiveScale();

		// go through the categories and see if this touch down was in one of them, to show it highlighted
		// TODO: uses similar code from below... really all this sidebar stuff shoould be refactored to use ListElement
		// and every category itself should be a CategoryLabel just like an AppCard consists of images + text
		for (int x = 0; x < TOTAL_CATS; x++)
		{
			int xc = 0,
				yc = (int)((150 + x * 70 - 15) * effectiveScale),
				// width = getWidth(),
				height = (int)(60 * effectiveScale);

			if (event->touchIn(xc, yc, width, height))
			{
				// touch is over an element of the sidebar, set the currentSelection
				currentSelection = x;

				if (event->isTouchDrag())
				{
					elasticCounter = THICK_HIGHLIGHT;
				}

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
		int previouslySelected = currentSelection;
		currentSelection = -1; // reset highlighted one

		float effectiveScale = getEffectiveScale();

		// check if it's one of the text elements
		for (int x = 0; x < TOTAL_CATS; x++)
		{
			int xc = 0,
				yc = (int)((150 + x * 70 - 15) * effectiveScale),
				// width = getWidth(),
				height = (int)(60 * effectiveScale); // TODO: extract formula into method (same as AppList x value)
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

	if (origHighlighted != highlighted)
	{
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
	auto effectiveScale = getEffectiveScale();

	// draw the light gray bg behind the active category
	CST_Rect dimens = { 0, 0, width, (int)(60 * effectiveScale) };					 // TODO: extract this to a method too
	dimens.y = 150 * effectiveScale + this->curCategory * 70 * effectiveScale - 15 * effectiveScale; // TODO: extract formula into method

	auto c = RootDisplay::mainDisplay->backgroundColor;
	// Add 0x25 to each component, clamping to 0xff max
	Uint8 rVal = (Uint8)std::min(255, (int)(c.r * 255) + 0x25);
	Uint8 gVal = (Uint8)std::min(255, (int)(c.g * 255) + 0x25);
	Uint8 bVal = (Uint8)std::min(255, (int)(c.b * 255) + 0x25);
	CST_Color consoleColor = { rVal, gVal, bVal, 0xff };
	CST_SetDrawColor(RootDisplay::renderer, consoleColor);

	if (this->showCurrentCategory)
		CST_FillRect(RootDisplay::renderer, &dimens);

	if (appList && appList->touchMode && (this->currentSelection >= 0 && this->elasticCounter != THICK_HIGHLIGHT))
	{
		CST_Rect dimens2 = { 0, 0, 400, 60 };
		dimens.y = 150 * effectiveScale + this->currentSelection * 70 * effectiveScale - 15 * effectiveScale; // TODO: extract formula into method
		CST_SetDrawBlend(RootDisplay::renderer, true);
		CST_Color highlight = { 0x10, 0xD9, 0xD9, 0x40 };
		CST_SetDrawColor(RootDisplay::renderer, highlight); // TODO: matches the DEEP_HIGHLIGHT color
		CST_FillRect(RootDisplay::renderer, &dimens2);
	}

	// draw the selected category, if one should be highlighted
	if (this->highlighted >= 0 || (this->currentSelection >= 0 && this->elasticCounter == THICK_HIGHLIGHT))
	{
		// for drag events, we want to use the thick highlight
		int highlightValue = (this->currentSelection >= 0 && this->elasticCounter == THICK_HIGHLIGHT) ? this->currentSelection : this->highlighted;

		int y = 150 * effectiveScale + highlightValue * 70 * effectiveScale - 15 * effectiveScale;
		//        rectangleRGBA(RootDisplay::renderer, 0, y, dimens.w, y + dimens.h, 0xff, 0x00, 0xff, 0xff);

		for (int x = -2; x < 3; x++)
		{
			CST_rectangleRGBA(MainDisplay::renderer,
				dimens.x + x, y + x,
				dimens.x + dimens.w - x, y + dimens.h - x,
				0x10, 0xD9, 0xD9, 0xff);
		}
	}

	// render subelements
	super::render(parent);
}

std::string Sidebar::currentCatName()
{
	if (this->curCategory >= 0 && this->curCategory < TOTAL_CATS)
		return i18n(this->cat_names[this->curCategory]);

	return std::string("?");
}

std::string Sidebar::currentCatValue()
{
	if (this->curCategory >= 0 && this->curCategory < TOTAL_CATS)
		return std::string(this->cat_value[this->curCategory]);

	return std::string("?");
}

#if defined(USE_OSC_BRANDING)
rgb getOSCCategoryColor(std::string category)
{
	if (category == "emulators")
	{
		return fromRGB(0x34, 0xED, 0x90);
	}
	else if (category == "games")
	{
		return fromRGB(0xED, 0x34, 0x9F);
	}
	else if (category == "utilities")
	{
		return fromRGB(0x34, 0xBE, 0xED);
	}
	else if (category == "media")
	{
		return fromRGB(0xff, 0xd3, 0x24);
	}
	else
	{
		return fromRGB(0xFF, 0xFF, 0xFF);
	}
}
#endif