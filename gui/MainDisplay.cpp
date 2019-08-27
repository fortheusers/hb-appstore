#if defined(SWITCH)
#include <switch.h>
#define PLATFORM "Switch"
#elif defined(__WIIU__)
#define PLATFORM "Wii U"
#else
#define PLATFORM "Console"
#endif

#include "libget/src/Utils.hpp"
#include "chesto/src/Button.hpp"

#include "MainDisplay.hpp"
#include "main.hpp"

MainDisplay::MainDisplay(Get* get)
{
	this->get = get;

	// redraw after first creating MainDisplay
	this->needsRedraw = true;

	// go through all repos and if one has an error, set the error flag
	bool atLeastOneEnabled = false;
	for (auto repo : this->get->repos)
	{
		this->error = this->error || !repo->loaded;
		atLeastOneEnabled = atLeastOneEnabled || repo->enabled;
	}
	this->error = this->error || !atLeastOneEnabled;

	// display error message
	if (this->error)
	{
		// create the first two elements (icon and app title)
		ImageElement* icon = new ImageElement(ROMFS "res/icon.png");
		icon->position(330 + this->error * 140, 255 - this->error * 230);
		icon->resize(70 - this->error * 35, 70 - this->error * 35);
		this->elements.push_back(icon);

		TextElement* title = new TextElement("Homebrew App Store", 50 - this->error * 25);
		title->position(415 + this->error * 100, 255 - this->error * 230);
		this->elements.push_back(title);

		std::string troubleshootingText = "No enabled repos found, check ./get/repos.json\nMake sure repo has at least one package";
		if (atLeastOneEnabled)
			troubleshootingText = std::string("Perform a connection test in the " PLATFORM " System Settings\nEnsure DNS isn't blocking: ") + this->get->repos[0]->url;

		TextElement* errorMessage = new TextElement("Couldn't connect to the Internet!", 40);
		errorMessage->position(345, 305);
		this->elements.push_back(errorMessage);

		TextElement* troubleshooting = new TextElement((std::string("Troubleshooting:\n") + troubleshootingText).c_str(), 20, NULL, false, 600);
		troubleshooting->position(380, 585);
		this->elements.push_back(troubleshooting);

		Button* btnQuit = new Button("Quit", SELECT_BUTTON, false, 15);
		btnQuit->position(1130, 630);
		btnQuit->action = quit;
		this->elements.push_back(btnQuit);

		return;
	}

	// add in the sidebar, footer, and main app listing
	Sidebar* sidebar = new Sidebar();
	this->elements.push_back(sidebar);

	AppList* applist = new AppList(this->get, sidebar);
	this->elements.push_back(applist);
	sidebar->appList = applist;
}
