#if defined(SWITCH)
#include <switch.h>
#define PLATFORM "Switch"
#elif defined(__WIIU__)
#include <romfs-wiiu.h>
#define PLATFORM "Wii U"
#else
#define PLATFORM "Console"
#endif

#include "libget/src/Utils.hpp"
#include "chesto/src/Button.hpp"

#include "MainDisplay.hpp"
#include "AppCard.hpp"

MainDisplay::MainDisplay(Get* get)
{
	// start up main chesto deps (images+text+font+sound+rendering)
	RootDisplay::super();

// initialize romfs for switch/wiiu
#if defined(SWITCH) || defined(__WIIU__)
	romfsInit();
#endif

	this->get = get;

	// populate image cache with any local version info if it exists
	this->imageCache = new ImageCache(get->tmp_path);

	// go through all repos and if one has an error, set the error flag
	bool atLeastOneEnabled = false;
	for (auto repo : this->get->repos)
	{
		this->error = this->error || !repo->loaded;
		atLeastOneEnabled = atLeastOneEnabled || repo->enabled;
	}

	this->error = this->error || !atLeastOneEnabled;

	// the progress bar
	ProgressBar* pbar = new ProgressBar();
	pbar->position(401, 380 - this->error * 290);
	this->elements.push_back(pbar);

	// the text above the progress bar
	//	TextElement* pbar_text = new TextElement("Updating App Info...", 17);
	//	pbar_text->position(550, 365);
	//	this->elements.push_back(pbar_text);

	// create the first two elements (icon and app title)
	ImageElement* icon = new ImageElement(ROMFS "res/icon.png");
	icon->position(330 + this->error * 140, 255 - this->error * 230);
	icon->resize(70 - this->error * 35, 70 - this->error * 35);
	this->elements.push_back(icon);

	TextElement* title = new TextElement("Homebrew App Store", 50 - this->error * 25);
	title->position(415 + this->error * 100, 255 - this->error * 230);
	this->elements.push_back(title);

	if (this->imageCache->version_cache.size() == 0)
	{
		notice = new TextElement("Still doing initial load-- next time will be faster!", 20);
		notice->position(410, 460);
		notice->hidden = true;
		this->elements.push_back(notice);
	}

	if (this->error)
	{
		std::string troubleshootingText = "No enabled repos found, check ./get/repos.json\nMake sure repo has at least one package";
		if (atLeastOneEnabled)
			troubleshootingText = std::string("Perform a connection test in the " PLATFORM " System Settings\nEnsure DNS isn't blocking: ") + this->get->repos[0]->url;

		TextElement* errorMessage = new TextElement("Couldn't connect to the Internet!", 40);
		errorMessage->position(345, 305);
		this->elements.push_back(errorMessage);

		TextElement* troubleshooting = new TextElement((std::string("Troubleshooting:\n") + troubleshootingText).c_str(), 20, NULL, false, 600);
		troubleshooting->position(380, 585);
		this->elements.push_back(troubleshooting);

		Button* quit = new Button("Quit", SELECT_BUTTON, false, 15);
		quit->position(1130, 630);
		quit->action = std::bind(&RootDisplay::exit, this);
		this->elements.push_back(quit);
	}
}

void MainDisplay::render(Element* parent)
{
	RootDisplay::render(parent);
}

bool MainDisplay::process(InputEvents* event)
{
	if (RootDisplay::subscreen == NULL)
	{
		// if we're on the splash/loading screen, we need to fetch icons+screenshots from the remote repo
		// and load them into our surface cache with the pkg_name+version as the key
		if (this->showingSplash && event->noop)
		{
			// should be a progress bar
			if (this->get->packages.size() != 1)
				((ProgressBar*)this->elements[0])->percent = (this->count / ((float)this->get->packages.size() - 1));

			// no packages, prevent crash TODO: display offline in bottom bar
			if (this->get->packages.size() == 0)
			{
				((ProgressBar*)this->elements[0])->percent = -1;
				this->showingSplash = false;
				return true;
			}

			if (notice && ((ProgressBar*)this->elements[0])->percent > 0.5)
				notice->hidden = false;

			// update the counter (TODO: replace with fetching app icons/screen previews)
			this->count++;

			// get the package whose icon+screen to process
			Package* current = this->get->packages[this->count - 1];

			// the path to the cache location of the icon and screen for this pkg_name and version number
			std::string key_path = imageCache->cache_path + current->pkg_name;

			// check if this package exists in our cache, but the version doesn't match
			// (if (it's not in the cache) OR (it's in the cache but the version doesn't match)
			if (this->imageCache->version_cache.count(current->pkg_name) == 0 || (this->imageCache->version_cache.count(current->pkg_name) && this->imageCache->version_cache[current->pkg_name] != current->version))
			{
				// the version in our cache doesn't match the one that will be on the server
				// so we need to download the images now
				mkdir(key_path.c_str(), 0700);

				bool success = downloadFileToDisk(*(current->repoUrl) + "/packages/" + current->pkg_name + "/icon.png", key_path + "/icon.png");
				if (!success) // manually add default icon to cache if downloading failed
					cp(ROMFS "res/default.png", (key_path + "/icon.png").c_str());
				// TODO: generate a custom icon for this version with a color and name

				// no more default banners, just try to download the file (don't do this on Wii U)
				#if !defined(__WIIU__)
				downloadFileToDisk(*(current->repoUrl) + "/packages/" + current->pkg_name + "/screen.png", key_path + "/screen.png");
				#endif

				// add these versions to the version map
				this->imageCache->version_cache[current->pkg_name] = current->version;
			}

			// whether we just downloaded it or it was already there from the cache, load this image element into our memory cache
			// (making an AppCard and calling update() will do this, even if we don't intend to do anything with it yet)
			AppCard a(current);
			a.update();

			// write the version we just got to the cache as well so that we can know whether or not we need to up date it next time

			// are we done processing all packages
			if (this->count == this->get->packages.size())
			{
				// write whatever we have in the icon version cache to a file
				this->imageCache->writeVersionCache();

				// remove the splash screen elements
				this->wipeElements();

				// add in the sidebar, footer, and main app listing
				Sidebar* sidebar = new Sidebar();
				this->elements.push_back(sidebar);

				AppList* applist = new AppList(this->get, sidebar);
				this->elements.push_back(applist);
				sidebar->appList = applist;

				this->showingSplash = false;
				this->needsRedraw = true;
			}

			return true;
		}
	}

	// parent stuff
	return RootDisplay::process(event);
}
