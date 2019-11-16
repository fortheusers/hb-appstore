#if defined(SWITCH)
#include <switch.h>
#define PLATFORM "Switch"
#elif defined(__WIIU__)
#define PLATFORM "Wii U"
#else
#define PLATFORM "Console"
#endif

#include "libget/src/Get.hpp"
#include "libget/src/Utils.hpp"
#include "chesto/src/Button.hpp"

#include "MainDisplay.hpp"
#include "main.hpp"

MainDisplay::MainDisplay()
{
	// draw a static splash screen while fetching repos metadata
	ImageElement* icon = new ImageElement(ROMFS "res/icon.png");
	icon->position(330, 255);
	icon->resize(70, 70);
	elements.push_back(icon);

	TextElement* title = new TextElement("Homebrew App Store", 50);
	title->position(415, 255);
	elements.push_back(title);

	TextElement* loading = new TextElement("Loading...", 40);
	loading->position(549, 365);
	elements.push_back(loading);

	this->error = this->error || !atLeastOneEnabled;

	#if defined(__WIIU__)
	this->backgroundColor={0.33, 0.33, 0.43};
	#else
	this->backgroundColor={0.26, 0.27, 0.28};
	#endif

	// initial redraw to show the splash screen
	needsRedraw = true;
}

void MainDisplay::drawErrorScreen(std::string troubleshootingText)
{
	wipeElements();

	ImageElement* icon = new ImageElement(ROMFS "res/icon.png");
	icon->position(470, 25);
	icon->resize(35, 35);
	this->elements.push_back(icon);

	TextElement* title = new TextElement("Homebrew App Store", 50 - 25);
	title->position(515, 25);
	this->elements.push_back(title);

	TextElement* errorMessage = new TextElement("Couldn't connect to the Internet!", 40);
	errorMessage->position(345, 305);
	elements.push_back(errorMessage);

	TextElement* troubleshooting = new TextElement((std::string("Troubleshooting:\n") + troubleshootingText).c_str(), 20, NULL, false, 600);
	troubleshooting->position(380, 585);
	elements.push_back(troubleshooting);

	Button* btnQuit = new Button("Quit", SELECT_BUTTON, false, 15);
	btnQuit->position(1130, 630);
	btnQuit->action = quit;
	elements.push_back(btnQuit);

	needsRedraw = true;
}

void MainDisplay::render(Element* parent)
{
	if (showingSplash)
		renderedSplash = true;
	RootDisplay::render(parent);
}

bool MainDisplay::process(InputEvents* event)
{
	if (!RootDisplay::subscreen && showingSplash && renderedSplash && event->noop)
	{
		showingSplash = false;

		// fetch repositories metadata
		get = new Get("./.get/", DEFAULT_REPO);

		// go through all repos and if one has an error, set the error flag
		for (auto repo : get->repos)
		{
			error = error || !repo->loaded;
			atLeastOneEnabled = atLeastOneEnabled || repo->enabled;
		}

		if (error)
		{
			drawErrorScreen(std::string("Perform a connection test in the " PLATFORM " System Settings\nEnsure DNS isn't blocking: ") + get->repos[0]->url);
			return true;
		}

		if (!atLeastOneEnabled)
		{
			drawErrorScreen("No enabled repos found, check ./get/repos.json\nMake sure repo has at least one package");
			return true;
		}

		// remove the splash screen elements
		wipeElements();

		// add in the sidebar, footer, and main app listing
		Sidebar* sidebar = new Sidebar();
		elements.push_back(sidebar);

		AppList* applist = new AppList(get, sidebar);
		elements.push_back(applist);
		sidebar->appList = applist;

		needsRedraw = true;

		return true;
	}

	// parent stuff
	return RootDisplay::process(event);
}
