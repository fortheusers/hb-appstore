#include <SDL2/SDL2_gfxPrimitives.h>
#include <fstream>
#include <sstream>

#if defined(SWITCH)
#include <switch.h>
#endif

#include "../libs/get/src/Get.hpp"
#include "../libs/get/src/Utils.hpp"

#include "../libs/chesto/src/RootDisplay.hpp"

#include "AppDetails.hpp"
#include "Feedback.hpp"
#include "AppList.hpp"
#include "ImageCache.hpp"
#include "main.hpp"

int AppDetails::lastFrameTime = 99;

AppDetails::AppDetails(Package* package, AppList* appList)
	: package(package)
	, get(appList->get)
	, appList(appList)
	, downloadProgress()
	, download(getAction(package), A_BUTTON, true, 30)
	, cancel("Cancel", B_BUTTON, true, 30, download.width)
	, details(getPackageDetails(package).c_str(), 20, &white, false, 300)
	, content(package)
	, downloadStatus("Downloading package...", 30)
{
	// TODO: show current app status somewhere

	// download/update/remove button (2)

	download.position(970, 550);
	download.action = std::bind(&AppDetails::proceed, this);

	cancel.position(970, 630);
	cancel.action = std::bind(&AppDetails::back, this);

#if defined(SWITCH)
	// display an additional launch/install button if the package is installed,  and has a binary or is a theme

	bool hasBinary = package->binary != "none";
	bool isTheme = this->package->category == "theme";

	if (package->status != GET && (hasBinary || isTheme))
	{
		download.position(970, 470);
		cancel.position(970, 630);

		const char* buttonLabel = "Launch";
		bool injectorPresent = false;

		if (isTheme)
		{
			Package* installer = get->lookup("NXthemes_Installer");
			injectorPresent = installer != NULL; // whether or not the currently hardcoded installer package exists, in the future becomes something functionality-based like "theme_installer"
			buttonLabel = (injectorPresent && installer->status == GET) ? "Injector" : "Inject";
		}

		// show the third button if a binary is present, or a theme injector is available (installed or not)
		if (hasBinary || injectorPresent)
		{
			this->canLaunch = true;

			start = new Button(buttonLabel, START_BUTTON, true, 30, download.width);
			start->position(970, 550);
			start->action = std::bind(&AppDetails::launch, this);
			super::append(start);
		}
	}
#endif

	super::append(&download);
	super::append(&cancel);

	// more details

	details.position(940, 50);
	super::append(&details);

	// the scrollable portion of the app details page
	content.moreByAuthor.action = std::bind(&AppDetails::moreByAuthor, this);
	content.reportIssue.action = std::bind(&AppDetails::leaveFeedback, this);
	super::append(&content);


	// download informations (not visible until the download is started)
	downloadStatus.position(10, 10);

	downloadProgress.width = 740;
	downloadProgress.position(1280 / 2 - downloadProgress.width / 2, 720 / 2 - 5);
	downloadProgress.color = 0xff0000ff;
	downloadProgress.dimBg = true;
}

AppDetails::~AppDetails()
{
#if defined(SWITCH)
	if (start)
	{
		super::remove(start);
		delete start;
	}
	if (errorText)
	{
		super::remove(errorText);
		delete errorText;
	}
#endif
}

std::string AppDetails::getPackageDetails(Package* package)
{
	// lots of details that we know about the package
	std::stringstream more_details;
	more_details << "Title: " << package->title << "\n"
	<< package->short_desc << "\n\n"
	<< "Author: " << package->author << "\n"
	<< "Version: " << package->version << "\n"
	<< "License: " << package->license << "\n\n"
	<< "Package: " << package->pkg_name << "\n"
	<< "Downloads: " << package->downloads << "\n"
	<< "Updated: " << package->updated << "\n\n"
	<< "Download size: " << package->download_size << " KB\n"
	<< "Install size: " << package->extracted_size << " KB\n";
	return more_details.str();
}

const char *AppDetails::getAction(Package* package)
{
	switch (package->status)
	{
		case GET:
			return "Download";
		case UPDATE:
			return "Update";
		case INSTALLED:
			return "Remove";
		case LOCAL:
			return "Reinstall";
		default:
			break;
	}
	return "?";
}

// TODO: make one push event function to bind instead of X separeate ones
void AppDetails::proceed()
{
	SDL_Event sdlevent;
	sdlevent.type = SDL_KEYDOWN;
	sdlevent.key.keysym.sym = SDLK_a;
	sdlevent.key.repeat = 0;
	SDL_PushEvent(&sdlevent);
}

void AppDetails::launch()
{
	if (!this->canLaunch)
		return;

	SDL_Event sdlevent;
	sdlevent.type = SDL_KEYDOWN;
	sdlevent.key.keysym.sym = SDLK_RETURN;
	sdlevent.key.repeat = 0;
	SDL_PushEvent(&sdlevent);
}

void AppDetails::getSupported()
{
	Package* installer = get->lookup("NXthemes_Installer");
	if (installer != NULL)
		RootDisplay::switchSubscreen(new AppDetails(installer, appList));
}

void AppDetails::back()
{
	SDL_Event sdlevent;
	sdlevent.type = SDL_KEYDOWN;
	sdlevent.key.keysym.sym = SDLK_b;
	sdlevent.key.repeat = 0;
	SDL_PushEvent(&sdlevent);
}

void AppDetails::moreByAuthor()
{
	const char* author = this->package->author.c_str();
	appList->sidebar->searchQuery = std::string(author);
	appList->sidebar->curCategory = 0;
	appList->update();
	appList->y = 0;
	appList->keyboard.hidden = true;
	RootDisplay::switchSubscreen(nullptr);
}

void AppDetails::leaveFeedback()
{
	RootDisplay::switchSubscreen(new Feedback(this->package));
}

bool AppDetails::process(InputEvents* event)
{

	// don't process any keystrokes if an operation is in progress
	if (this->operating)
		return false;

	if (event->pressed(B_BUTTON))
	{
		RootDisplay::switchSubscreen(nullptr);
		return true;
	}

	if (event->pressed(A_BUTTON))
	{
		this->operating = true;
		// event->key.keysym.sym = SDLK_z;
		event->update();

		// description of what we're doing
		super::append(&downloadStatus);
		super::append(&downloadProgress);

		// setup progress bar callback
		networking_callback = AppDetails::updateCurrentlyDisplayedPopup;

		// if we're installing ourselves, we need to quit after on switch
		preInstallHook();

		// install or remove this package based on the package status
		if (this->package->status == INSTALLED)
			get->remove(this->package);
		else
			get->install(this->package);

		postInstallHook();

		// refresh the screen
		RootDisplay::switchSubscreen(nullptr);

		this->operating = false;
		this->appList->update();
		return true;
	}

#if defined(SWITCH)
	if (event->pressed(START_BUTTON) && this->canLaunch == true)
	{
		char path[8 + strlen(package->binary.c_str())];

		sprintf(path, "sdmc:/%s", package->binary.c_str());
		printf("Launch path: %s\n", path);

		FILE* file;
		bool successLaunch = false;

		if (package->category == "theme")
		{
			Package* installer = get->lookup("NXthemes_Installer"); // This should probably be more dynamic in future, e.g. std::vector<Package*> Get::find_functionality("theme_installer")
			if (installer != NULL && installer->status != GET)
			{
				sprintf(path, "sdmc:/%s", installer->binary.c_str());
				successLaunch = this->themeInstall(path);
			}
			else
			{
				successLaunch = true;
				this->getSupported();
			}
		}
		else
		{
			//Final check if path actually exists
			if ((file = fopen(path, "r")))
			{
				fclose(file);
				printf("Path OK, Launching...");
				successLaunch = this->launchFile(path, path);
			}
			else
				successLaunch = false;
		}

		if (!successLaunch)
		{
			printf("Failed to launch.");
			errorText = new TextElement("Couldn't launch app", 24, &red, false, 300);
			errorText->position(970, 430);
			super::append(errorText);
			this->canLaunch = false;
		}
		return true;
	}
#endif

	if (event->isTouchDown())
		this->dragging = true;

	// if A or B were hit, we don't get down here (which is good, because the children buttons are just pushing A and B events)
	return super::process(event);
}

void AppDetails::preInstallHook()
{
  // is there an echo in here?
}

bool AppDetails::themeInstall(char* installerPath)
{
	std::string ManifestPathInternal = "manifest.install";
	std::string ManifestPath = get->pkg_path + this->package->pkg_name + "/" + ManifestPathInternal;

	std::vector<std::string> themePaths;

	if (!package->manifest) package->manifest = new Manifest(ManifestPath, ROOT_PATH);

	if (package->manifest->valid)
	{
		for (size_t i = 0; i <= package->manifest->entries.size() - 1; i++)
		{
			if (package->manifest->entries[i].operation == MUPDATE && package->manifest->entries[i].extension == "nxtheme")
			{
				printf("Found nxtheme\n");
				themePaths.push_back(package->manifest->entries[i].path);
			}
		}

	}else{
		printf("--> ERROR: no manifest found/manifest invalid at %s\n", ManifestPath.c_str());
		return false;
	}

	std::string themeArg = "installtheme=";
	for (int i = 0; i < (int)themePaths.size(); i++)
	{
		if (i == (int)themePaths.size() - 1)
		{
			themeArg.append(themePaths[i]);
		}
		else
		{
			themeArg.append(themePaths[i]);
			themeArg.append(",");
		}
	}
	printf("Theme Install: %s\n", themeArg.c_str());
	size_t index = 0;
	while (true)
	{
		index = themeArg.find(" ", index);
		if (index == std::string::npos) break;
		themeArg.replace(index, 1, "(_)");
	}
	char args[strlen(installerPath) + themeArg.size() + 8];
	sprintf(args, "%s %s", installerPath, themeArg.c_str());
	return this->launchFile(installerPath, args);
}

bool AppDetails::launchFile(char* path, char* context)
{
#if defined(SWITCH)
	//If setnexload works without problems, quit to make loader open next nro
	if (R_SUCCEEDED(envSetNextLoad(path, context)))
	{
		quit();
		return true;
	}
	return false;
#endif
	return false;
}

void AppDetails::postInstallHook()
{
  // nothing!!
}

void AppDetails::render(Element* parent)
{
	if (this->renderer == NULL)
		this->renderer = parent->renderer;
	if (this->parent == NULL)
		this->parent = parent;

	// draw white background
	SDL_Rect dimens = { 0, 0, 920, 720 };

	SDL_SetRenderDrawColor(parent->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(parent->renderer, &dimens);

	SDL_SetRenderDrawColor(parent->renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	// draw all elements
	super::render(this);
}

int AppDetails::updateCurrentlyDisplayedPopup(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	int now = SDL_GetTicks();
	int diff = now - AppDetails::lastFrameTime;

	if (dltotal == 0) dltotal = 1;

	double amount = dlnow / dltotal;

	// don't update the GUI too frequently here, it slows down downloading
	// (never return early if it's 100% done)
	if (diff < 32 && amount != 1)
		return 0;

	AppDetails* popup = (AppDetails*)RootDisplay::subscreen;

	// update the amount
	if (popup != NULL)
	{
		popup->downloadProgress.percent = amount;

		// force render the element right here (and it's progress bar too)
		if (popup->parent != NULL)
		{
			InputEvents* events = new InputEvents();
			while (events->update())
				popup->parent->process(events);
			popup->parent->render(NULL);
		}
	}

	AppDetails::lastFrameTime = SDL_GetTicks();

	return 0;
}

AppDetailsContent::AppDetailsContent(Package *package)
	: reportIssue("Report Issue", Y_BUTTON)
	, moreByAuthor("More by Author", X_BUTTON)
	, title(package->title.c_str(), 35, &black)
	, title2(package->author.c_str(), 27, &gray)
	, details(package->long_desc.c_str(), 20, &black, false, 740)
	, changelog((std::string("Changelog:\n") + package->changelog).c_str(), 20, &black, false, 740)
#if defined(__WIIU__) // Use an icon banner
	, banner(package->getIconUrl().c_str(), []{
			// if the icon fails to load, use the default icon
			ImageElement *defaultIcon = new ImageElement(RAMFS "res/default.png");
			defaultIcon->setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
			return defaultIcon;
		})
#else // Load the banner from network
	, banner(package->getBannerUrl().c_str(), [package]{
			// If the banner fails to load, use an icon banner
			NetImageElement* icon = new NetImageElement(package->getIconUrl().c_str(), []{
				// if even the icon fails to load, use the default icon
				ImageElement *defaultIcon = new ImageElement(RAMFS "res/default.png");
				defaultIcon->setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
				return defaultIcon;
			});
			icon->setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
			return icon;
		})
#endif
{
	title.position(MARGIN, 30);
	super::append(&title);

	reportIssue.position(920 - MARGIN - reportIssue.width, 45);
	super::append(&reportIssue);

	moreByAuthor.position(reportIssue.x - 20 - moreByAuthor.width, 45);
	super::append(&moreByAuthor);

#if defined(__WIIU__)
	banner.setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
#endif
	banner.resize(787, 193);
	banner.position(BANNER_X, BANNER_Y);
	super::append(&banner);

	title2.position(MARGIN, 80);
	super::append(&title2);

	// the main description (wrapped text)
	details.position(MARGIN + 30, banner.y + banner.height + 22);
	super::append(&details);

	changelog.position(MARGIN + 30, details.y + details.height + 30);
	super::append(&changelog);
}

void AppDetailsContent::render(Element* parent)
{
	if (this->parent == NULL)
		this->parent = parent;

	this->renderer = parent->renderer;

	super::render(this);
}

bool AppDetailsContent::process(InputEvents* event)
{
	int SPEED = 60;
	bool ret = false;

	// handle up and down for the scroll view
	if (event->isKeyDown())
	{
		// scroll the view
		this->y += (SPEED * event->held(UP_BUTTON) - SPEED * event->held(DOWN_BUTTON));
		if (this->y > 0)
			this->y = 0;
		ret |= event->held(UP_BUTTON) || event->held(DOWN_BUTTON);
	}

	return ret || ListElement::process(event);
}
