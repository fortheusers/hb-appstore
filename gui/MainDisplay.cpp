#if defined(SWITCH)
#include <switch.h>
#define PLATFORM "Switch"
#elif defined(__WIIU__)
#define PLATFORM "Wii U"
#else
#define PLATFORM "Console"
#endif

#include "../libs/get/src/Get.hpp"
#include "../libs/get/src/Utils.hpp"

#include "MainDisplay.hpp"
#include "main.hpp"

MainDisplay::MainDisplay()
	: appList(NULL, &sidebar)
{
	// add in the sidebar, footer, and main app listing
	sidebar.appList = &appList;

	super::append(&sidebar);
	super::append(&appList);

	needsRedraw = true;
}

MainDisplay::~MainDisplay()
{
	delete get;
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

		networking_callback = MainDisplay::updateLoader;

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
			RootDisplay::switchSubscreen(new ErrorScreen(std::string("Perform a connection test in the " PLATFORM " System Settings\nEnsure DNS isn't blocking: ") + get->repos[0]->url));
			return true;
		}

		if (!atLeastOneEnabled)
		{
			RootDisplay::switchSubscreen(new ErrorScreen("No enabled repos found, check ./get/repos.json\nMake sure repo has at least one package"));
			return true;
		}

		networking_callback = nullptr;

		// set get instance to our applist
		appList.get = get;
		appList.update();
		appList.sidebar->addHints();

		return true;
	}

	// parent stuff
	return RootDisplay::process(event);
}


int MainDisplay::updateLoader(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	int now = SDL_GetTicks();
	int diff = now - AppDetails::lastFrameTime;

	if (dltotal == 0) dltotal = 1;

	double amount = dlnow / dltotal;

	// don't update the GUI too frequently here, it slows down downloading
	// (never return early if it's 100% done)
	if (diff < 32 && amount != 1)
		return 0;

	MainDisplay* display = (MainDisplay*)RootDisplay::mainDisplay;
	if (display->appList.spinner != NULL)
		display->appList.spinner->angle += 10;
	display->render(NULL);

	AppDetails::lastFrameTime = SDL_GetTicks();

	return 0;
}


ErrorScreen::ErrorScreen(std::string troubleshootingText)
	: icon(RAMFS "res/icon.png")
	, title("Homebrew App Store", 50 - 25)
	, errorMessage("Couldn't connect to the Internet!", 40)
	, troubleshooting((std::string("Troubleshooting:\n") + troubleshootingText).c_str(), 20, NULL, false, 600)
	, btnQuit("Quit", SELECT_BUTTON, false, 15)
{
	icon.position(470, 25);
	icon.resize(35, 35);
	title.position(515, 25);
	errorMessage.position(345, 305);
	troubleshooting.position(380, 585);
	btnQuit.position(1130, 630);

	btnQuit.action = quit;

	super::append(&icon);
	super::append(&title);
	super::append(&errorMessage);
	super::append(&troubleshooting);
	super::append(&btnQuit);
}
