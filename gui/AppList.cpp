#include "AppList.hpp"
#include "AboutScreen.hpp"
#include "Keyboard.hpp"
#include "main.hpp"

#include "../libs/get/src/Utils.hpp"

#include "../libs/chesto/src/RootDisplay.hpp"

#include <SDL2/SDL2_gfxPrimitives.h>

#include <algorithm>
#include <cstdlib> // std::rand, std::srand
#include <ctime>   // std::time

const char* AppList::sortingDescriptions[TOTAL_SORTS] = { "by most recent", "by download count", "alphabetically", "by size (descending)", "randomly" };
SDL_Color AppList::black = { 0, 0, 0, 0xff };
SDL_Color AppList::gray = { 0x50, 0x50, 0x50, 0xff };

AppList::AppList(Get* get, Sidebar* sidebar)
	: get(get)			// the main get instance that contains repo info and stuff
	, sidebar(sidebar)	// the sidebar, which will store the currently selected category info
	, keyboard(this, &this->sidebar->searchQuery)
	, quitBtn("Quit", SELECT_BUTTON, false, 15)
	, creditsBtn("Credits", X_BUTTON, false, 15)
	, sortBtn("Adjust Sort", Y_BUTTON, false, 15)
	, keyboardBtn("Toggle Keyboard", Y_BUTTON, false, 15)
#if defined(MUSIC)
	, muteBtn(" ", 0, false, 15, 43)
	, muteIcon(RAMFS "res/mute.png")
#endif
{
	this->x = 400 - 260 * (R - 3);

	// the offset of how far along scroll'd we are
	this->y = 0;

	// initialize random numbers used for sorting
	std::srand(unsigned(std::time(0)));

	// quit button
	quitBtn.action = quit;

	// additional buttons
	creditsBtn.action = std::bind(&AppList::launchSettings, this);
	sortBtn.action = std::bind(&AppList::cycleSort, this);
#if defined(MUSIC)
	muteBtn.action = std::bind(&AppList::toggleAudio, this);
	muteIcon.resize(32, 32);
#endif

	// search buttons
	keyboardBtn.action = std::bind(&AppList::toggleKeyboard, this);

	// initial loading spinner
	spinner = new ImageElement(RAMFS "res/spinner.png");
	spinner->position(395, 90);
	spinner->resize(90, 90);
	super::append(spinner);

	// update current app listing
	update();
}

bool AppList::process(InputEvents* event)
{
	bool ret = false;

	if (event->pressed(ZL_BUTTON) || event->pressed(L_BUTTON))
	{
		R = (R == 3) ? 4 : 3;
		this->x = 400 - 260 * (R - 3);
		update();
		return true;
	}

	// must be done before keyboard stuff to properly switch modes
	if (event->isTouchDown())
	{
		// remove a highilight if it exists (TODO: same as an above if statement)
		if (this->highlighted >= 0 && this->highlighted < this->elements.size() && this->elements[this->highlighted])
			this->elements[this->highlighted]->elasticCounter = NO_HIGHLIGHT;

		// got a touch, so let's enter touchmode
		this->highlighted = -1;
		this->touchMode = true;
	}

	// if we're showing a keyboard, make sure we're not in its bounds
	// also make sure the children elements exist before trying the keyboard
	// AND we're actually on the search category
	// also if we're not in touchmode, always go in here regardless of any button presses (user can only interact with keyboard)
	bool keyboardIsShowing = !spinner && sidebar && sidebar->curCategory == 0 && !keyboard.hidden;
	if (keyboardIsShowing && ((event->isTouchDown() && event->touchIn(keyboard.x, keyboard.y, keyboard.width, keyboard.height)) || !touchMode))
	{
		ret |= keyboard.process(event);
		if (event->isKeyDown() && event->held(Y_BUTTON))
			ret |= ListElement::process(event); // continue processing ONLY if they're pressing Y
		return ret;
	}

	int origHighlight = this->highlighted;

	// process some joycon input events
	if (event->isKeyDown())
	{
		if (keyboardIsShowing)
		{
			// keyboard is showing, but we'r epressing buttons, and we're down here, so set touch mode and get out
			touchMode = false;
			if (event->held(Y_BUTTON)) // again, only let a Y through to toggle keyboard (TODO: redo this!)
				ret |= ListElement::process(event);
			return true; // short circuit, should be handled by someone else
		}

		if (event->held(A_BUTTON | B_BUTTON | UP_BUTTON | DOWN_BUTTON | LEFT_BUTTON | RIGHT_BUTTON))
		{
			// if we were in touch mode, draw the cursor in the applist
			// and reset our position
			if (this->touchMode)
			{
				this->touchMode = false;
				this->highlighted = 0;
				this->y = 0;		 // reset scroll TODO: maintain scroll when switching back from touch mode
				event->keyCode = -1; // we already have the cursor where we want it, no further updates
				ret |= true;
			}

			if (event->held(A_BUTTON) && this->highlighted >= 0)
			{
				this->elements[this->highlighted]->action();
				ret |= true;
			}

			// touchmode is false, but our highlight value is negative
			// (do nothing, let sidebar update our highlight value)
			if (this->highlighted < 0) return false;

			// look up whatever is currently chosen as the highlighted position
			// and remove its highlight
			if (this->elements[this->highlighted])
				this->elements[this->highlighted]->elasticCounter = NO_HIGHLIGHT;

			// if we got a LEFT key while on the left most edge already, transfer to categories
			if (this->highlighted % R == 0 && event->held(LEFT_BUTTON))
			{
				this->highlighted = -1;
				this->sidebar->highlighted = this->sidebar->curCategory;
				return true;
			}

			// similarly, prevent a RIGHT from wrapping to the next line
			if (this->highlighted % R == (R - 1) && event->held(RIGHT_BUTTON)) return false;

			// adjust the cursor by 1 for left or right
			this->highlighted += -1 * (event->held(LEFT_BUTTON)) + (event->held(RIGHT_BUTTON));

			// adjust it by R for up and down
			this->highlighted += -1 * R * (event->held(UP_BUTTON)) + R * (event->held(DOWN_BUTTON));

			// don't let the cursor go out of bounds
			if (this->highlighted >= (int)this->elements.size()) this->highlighted = this->elements.size() - 1;

			if (this->highlighted < 0) this->highlighted = 0;
			if (this->highlighted >= (int)this->totalCount) this->highlighted = this->totalCount - 1;
		}
	}

	// always check the currently highlighted piece and try to give it a thick border or adjust the screen
	if (!touchMode && this->elements.size() > this->highlighted && this->highlighted >= 0 && this->elements[this->highlighted])
	{
		// if our highlighted position is large enough, force scroll the screen so that our cursor stays on screen

		Element* curTile = this->elements[this->highlighted];

		// the y-position of the currently highlighted tile, precisely on them screen (accounting for scroll)
		// this means that if it's < 0 or > 720 then it's not visible
		int normalizedY = curTile->y + this->y;

		// if we're out of range above, recenter at the top row
		if (normalizedY < 0)
			this->y = -1 * (curTile->y - 15) + 25;

		// if we're out of range below, recenter at bottom row
		if (normalizedY > 720 - curTile->height)
			this->y = -1 * (curTile->y - 3 * (curTile->height - 15)) - 40;

		// if the card is this close to the top, just set it the list offset to 0 to scroll up to the top
		if (this->highlighted < R)
			this->y = 0;

		if (this->elements[this->highlighted] && this->elements[this->highlighted]->elasticCounter == NO_HIGHLIGHT)
		{
			this->elements[this->highlighted]->elasticCounter = THICK_HIGHLIGHT;
			ret |= true;
		}
	}

	// highlight was modified, we need to redraw
	if (origHighlight != this->highlighted)
		ret |= true;

	ret |= ListElement::process(event);

	return ret;
}

AppList::~AppList()
{
	delete spinner;
	delete category;
	delete sortBlurb;
}

void AppList::render(Element* parent)
{
	if (this->parent == NULL)
		this->parent = parent;

	// draw a white background, 870 wide
	SDL_Rect dimens = { 0, 0, 920 + 260 * (R - 3), 720 };
	dimens.x = this->x - 35;

  if (parent != NULL) {
    SDL_SetRenderDrawColor(parent->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(parent->renderer, &dimens);
    this->renderer = parent->renderer;
  }

	super::render(this);
}

bool AppList::sortCompare(const Package* left, const Package* right)
{
	// handle the supported sorting modes
	switch (sortMode)
	{
		case ALPHABETICAL:
			return left->title.compare(right->title) < 0;
		case POPULARITY:
			return left->downloads > right->downloads;
		case SIZE:
			return left->download_size > right->download_size;
		case RECENT:
			break;
		default:
			break;
	}

	// RECENT sort order is the default view, so it puts updates and installed apps first
	auto statusPriority = [](int status)->int
	{
		switch (status)
		{
			case UPDATE:	return 0;
			case INSTALLED:	return 1;
			case LOCAL:		return 2;
			case GET:		return 3;
		}
		return 4;
	};
	int priorityLeft = statusPriority(left->status);
	int priorityRight = statusPriority(right->status);

	if (priorityLeft == priorityRight)
		return left->updated_timestamp > right->updated_timestamp;

	return priorityLeft < priorityRight;
}

void AppList::update()
{
	if (!get)
		return;

	// remove elements
	super::removeAll();

	// destroy old elements
	appCards.clear();
	delete spinner;
	spinner = nullptr;
	delete category;
	category = nullptr;
	delete sortBlurb;
	sortBlurb = nullptr;

	// the current category value from the sidebar
	std::string curCategoryValue = sidebar->currentCatValue();

	// all packages TODO: move some of this filtering logic into main get library
	// if it's a search, do a search query through get rather than using all packages
	std::vector<Package*> packages = (curCategoryValue == "_search")
		? get->search(sidebar->searchQuery)
		: get->packages;

	// sort the packages
	if (sortMode == RANDOM)
		std::shuffle(packages.begin(), packages.end(), randDevice);
	else
		std::sort(packages.begin(), packages.end(), std::bind(&AppList::sortCompare, this, std::placeholders::_1, std::placeholders::_2));

	// add AppCards for the packages belonging to the current category
	for (auto &package : packages)
	{
		if (curCategoryValue == "_misc")
		{
			// if we're on misc, filter out packages belonging to some category
			if (std::find(std::begin(sidebar->cat_value), std::end(sidebar->cat_value), package->category) != std::end(sidebar->cat_value))
				continue;
		}
		else if (curCategoryValue != "_all" && curCategoryValue != "_search")
		{
			// if we're in a specific category, filter out package of different categories
			if (curCategoryValue != package->category)
				continue;
		}

		// create and position the AppCard for the package
		appCards.emplace_back(package, this);
		AppCard& card = appCards.back();
		card.index = appCards.size() - 1;
		card.position(25 + (card.index % R) * 265, 145 + (card.height + 15) * (card.index / R));
		card.update();
		super::append(&card);
	}
	totalCount = appCards.size();

	// add quit button
	quitBtn.position(720 + 260 * (R - 3), 70);
	super::append(&quitBtn);

	// update the view for the current category
	if (curCategoryValue == "_search")
	{
		// add the keyboard
		keyboardBtn.position(quitBtn.x - 20 - keyboardBtn.width, quitBtn.y);
		super::append(&keyboardBtn);
		keyboard.position(372 + (3 - R) * 132, 417);
		super::append(&keyboard);

		// category text
		category = new TextElement((std::string("Search: \"") + sidebar->searchQuery + "\"").c_str(), 28, &black);
		category->position(20, 90);
		super::append(category);
	}
	else
	{
		// add additional buttons
		creditsBtn.position(quitBtn.x - 20 - creditsBtn.width, quitBtn.y);
		super::append(&creditsBtn);
		sortBtn.position(creditsBtn.x - 20 - sortBtn.width, quitBtn.y);
		super::append(&sortBtn);
#if defined(MUSIC)
		muteBtn.position(sortBtn.x - 20 - muteBtn.width, quitBtn.y);
		super::append(&muteBtn);
		muteIcon.position(sortBtn.x - 20 - muteBtn.width + 5, quitBtn.y + 5);
		super::append(&muteIcon);
#endif

		// category text
		category = new TextElement(sidebar->currentCatName().c_str(), 28, &black);
		category->position(20, 90);
		super::append(category);

		// add the search type next to the category in a gray font
		sortBlurb = new TextElement(sortingDescriptions[sortMode], 15, &gray);
		sortBlurb->position(category->x + category->width + 15, category->y + 12);
		super::append(sortBlurb);
	}
}

void AppList::reorient()
{
	// remove a highilight if it exists (TODO: extract method, we use this everywehre)
	if (this->highlighted >= 0 && this->highlighted < this->elements.size() && this->elements[this->highlighted])
		this->elements[this->highlighted]->elasticCounter = NO_HIGHLIGHT;
}

void AppList::cycleSort()
{
	reorient();
	sortMode = (sortMode + 1) % TOTAL_SORTS;
	update();
}

void AppList::toggleAudio()
{
#if defined(MUSIC)
	if (Mix_PausedMusic())
		Mix_ResumeMusic();
	else
		Mix_PauseMusic();
#endif
}

void AppList::toggleKeyboard()
{
	reorient();
	keyboard.hidden = !keyboard.hidden;

	// if it's hidden now, make sure we release our highlight
	if (keyboard.hidden)
	{
		sidebar->highlighted = -1;
		highlighted = 0;
	}

	needsRedraw = true;
}

void AppList::launchSettings()
{
	RootDisplay::switchSubscreen(new AboutScreen(this->get));
}
