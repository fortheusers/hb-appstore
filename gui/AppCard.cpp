#include "AppCard.hpp"
#include "AppList.hpp"
#include "MainDisplay.hpp"
#include "ThemeManager.hpp"

AppCard::AppCard(Package& package, AppList* list)
	: package(std::make_shared<Package>(package))
	, list(list)
{
	icon = createNode<NetImageElement>(package.getIconUrl().c_str(), [list, package]
		{
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

		return new ImageElement(RAMFS "res/default.png"); }, !list);

	auto textSize = 13 * getEffectiveScale();

	version = createNode<TextElement>(("v" + package.getVersion()).c_str(), textSize, &HBAS::ThemeManager::textSecondary);
	status = createNode<TextElement>(package.statusString(), textSize, &HBAS::ThemeManager::textSecondary);
	appname = createNode<TextElement>(package.getTitle().c_str(), textSize + 3, &HBAS::ThemeManager::textPrimary);
	author = createNode<TextElement>(package.getAuthor().c_str(), textSize, &HBAS::ThemeManager::textSecondary);
	statusicon = createNode<ImageElement>((RAMFS "res/" + std::string(package.statusString()) + ".png").c_str());

	// fixed width+height of one app card
	this->width = 256;  // + 9px margins
	this->height = ICON_SIZE + 45;

	this->touchable = true;

	// connect the action to the callback for this element, to be invoked when the touch event fires
	this->action = std::bind(&AppCard::displaySubscreen, this);

	icon->setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
	icon->backgroundColor = fromRGB(0xFF, 0xFF, 0xFF);

#if defined(WII) || defined(WII_MOCK)
	icon->resize(128, 48);
	width = 170;
	height = 110;
	// on wii we'll use differently sized icons
	icon->cornerRadius = cornerRadius = 25;
	icon->setScaleMode(SCALE_PROPORTIONAL_NO_BG);
	icon->backgroundColor = fromRGB(0x67, 0x67, 0x67); // dark gray on wii, where missing background colors is common

	// set the bg color based on the category color
#if defined(USE_OSC_BRANDING)
	auto color = getOSCCategoryColor(package.getCategory());
	backgroundColor = color;
	hasBackground = true;
#endif
#elif defined(_3DS) || defined(_3DS_MOCK)
	icon->resize(ICON_SIZE, ICON_SIZE);
	this->width = 85;
#else
	icon->resize(256, ICON_SIZE);
#endif
	float effectiveScale = getEffectiveScale();
	statusicon->resize(30 * effectiveScale, 30 * effectiveScale);
}

void AppCard::update()
{
	int w, h;

	// update the position of the elements

	icon->position(0, 0);
	version->position(40, icon->height + 10);
	status->position(40, icon->height + 25);

	int spacer = this->width - 11; // 245 on 720p

	appname->getTextureSize(&w, &h);
	appname->position(spacer - w, icon->height + 5);

	author->getTextureSize(&w, &h);
	author->position(spacer - w, icon->height + 25);

	statusicon->position(4, icon->height + 10);
}

void AppCard::displaySubscreen()
{
	if (!list)
		return;

	// received a click on this app, push it onto the screen stack
	auto appDetails = std::make_unique<AppDetails>(*package, list, this);

	if (!list->touchMode)
		appDetails->highlighted = 0; // show cursor if we're not in touch mode

	RootDisplay::pushScreen(std::move(appDetails));
}

AppCard::~AppCard()
{
}