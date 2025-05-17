#include "AppCard.hpp"
#include "AppList.hpp"
#include "ThemeManager.hpp"
#include "MainDisplay.hpp"

#define TEXT_SIZE	13 / SCALER

AppCard::AppCard(Package& package, AppList* list)
	: package(new Package(package))
	, list(list)
	, icon(package.getIconUrl().c_str(), [list, package] {
		// if the icon fails to load, and we're offline, try to use one from the cache
		auto iconSavePath = std::string(list->get->mPkg_path) + "/" + package.getPackageName() + "/icon.png";

		// check if the package is installed, and if the icon file exists using stat
		struct stat buffer;
		if (package.getStatus() != GET && stat(iconSavePath.c_str(), &buffer) == 0) {
			// file exists, return the path to the icon
			auto img = new ImageElement(iconSavePath.c_str());
			img->setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
			return img;
		}

		return new ImageElement(RAMFS "res/default.png");
	}, !list)
	, version(("v" + package.getVersion()).c_str(), TEXT_SIZE, &HBAS::ThemeManager::textSecondary)
	, status(package.statusString(), TEXT_SIZE, &HBAS::ThemeManager::textSecondary)
	, appname(package.getTitle().c_str(), TEXT_SIZE + 3, &HBAS::ThemeManager::textPrimary)
	, author(package.getAuthor().c_str(), TEXT_SIZE, &HBAS::ThemeManager::textSecondary)
	, statusicon((RAMFS "res/" + std::string(package.statusString()) + ".png").c_str())
{
	// fixed width+height of one app card
	this->width = 256;  // + 9px margins
	this->height = ICON_SIZE + 45;

	this->touchable = true;

	// connect the action to the callback for this element, to be invoked when the touch event fires
	this->action = std::bind(&AppCard::displaySubscreen, this);

	icon.setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
	icon.backgroundColor = fromRGB(0xFF, 0xFF, 0xFF);

#if defined(WII) || defined(WII_MOCK)
	// on wii we'll use differently sized icons
	icon.cornerRadius = cornerRadius = 25;
	icon.setScaleMode(SCALE_PROPORTIONAL_NO_BG);
	icon.backgroundColor = fromRGB(0x67, 0x67, 0x67); // dark gray on wii, where missing background colors is common

	// set the bg color based on the category color
#if defined(USE_OSC_BRANDING)
	auto color = getOSCCategoryColor(package.getCategory());
	backgroundColor = color;
	hasBackground = true;
#endif
#endif
	
#if defined(_3DS) || defined(_3DS_MOCK)
	icon.resize(ICON_SIZE, ICON_SIZE);
  this->width = 85;
#else
  icon.resize(256, ICON_SIZE);
#endif
	statusicon.resize(30 / SCALER, 30 / SCALER);

	super::append(&icon);

#if !defined(_3DS) && !defined(_3DS_MOCK)
	super::append(&version);
	super::append(&status);
#endif

	super::append(&appname);
	super::append(&author);
	super::append(&statusicon);
}

void AppCard::update()
{
	int w, h;

	// update the position of the elements

	icon.position(0, 0);
	version.position(40, icon.height + 10);
	status.position(40, icon.height + 25);

  	int spacer = this->width - 11; // 245 on 720p

	appname.getTextureSize(&w, &h);
	appname.position(spacer - w, icon.height + 5);

	author.getTextureSize(&w, &h);
	author.position(spacer - w, icon.height + 25);

	statusicon.position(4, icon.height + 10);
}

// Trigger the icon download (if the icon wasn't already cached)
// when the icon is near the visible part of the screen
void AppCard::handleIconLoad()
{
	if (iconFetch)
		return;
	
	// printf("Y position: %d, %d, %d, %d - %s\n", list->y, this->y, this->height, SCREEN_HEIGHT, package.getTitle().c_str());

	// don't try to load the icon if the card is not visible ("on screen, within height (one card)")
	CST_Rect rect = { this->xOff + this->x, this->yOff + this->y - this->height, this->width, this->height };
	if (CST_isRectOffscreen(&rect))
		return;

	// the icon is either visible or ofscreen within 2 rows,
	// so the download can be started
	icon.fetch();

	// printf("Fetching icon for %s\n", package.getTitle().c_str());

	iconFetch = true;
}

void AppCard::render(Element* parent)
{
	this->xOff = parent->x;
	this->yOff = parent->y;

	// TODO: don't render this card if it's going to be offscreen anyway according to the parent (AppList)
	CST_Rect rect = { this->xOff + this->x, this->yOff + this->y, this->width, this->height };
	if (CST_isRectOffscreen(&rect))
		return;

	// render all the subelements of this card
	super::render(parent);
}

void AppCard::displaySubscreen()
{
	if (!list)
		return;

	// received a click on this app, add a subscreen under the parent
	AppDetails *appDetails = new AppDetails(*package, list, this);

	if (!list->touchMode)
		appDetails->highlighted = 0; // show cursor if we're not in touch mode

	MainDisplay::switchSubscreen(appDetails);
}

bool AppCard::process(InputEvents* event)
{
	if (list)
	{
		handleIconLoad();

		this->xOff = this->list->x;
		this->yOff = this->list->y;
	}

	return super::process(event);
}

AppCard::~AppCard()
{
	delete package;
}