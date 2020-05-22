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
	, download(getAction(package), A_BUTTON, true, 30 / SCALER)
	, cancel("Cancel", B_BUTTON, true, 30 / SCALER, download.width)
	, details(getPackageDetails(package).c_str(), 20, &white, false, 300)
	, content(package, appList->useBannerIcons)
	, downloadStatus("Download Status", 30 / SCALER, &white)
{
	// TODO: show current app status somewhere

	// download/update/remove button (2)

	download.position(970, 550);
	download.action = std::bind(&AppDetails::proceed, this);

	cancel.position(970, 630);
	cancel.action = std::bind(&AppDetails::back, this);

#if defined(_3DS) || defined(_3DS_MOCK)
  download.position(SCREEN_WIDTH / 2 - download.width / 2, 360);
  cancel.position(SCREEN_WIDTH / 2 - cancel.width / 2, 410);
#endif

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

	// more details

	details.position(940, 50);
	super::append(&details);

	// the scrollable portion of the app details page
	content.moreByAuthor.action = std::bind(&AppDetails::moreByAuthor, this);
	content.reportIssue.action = std::bind(&AppDetails::leaveFeedback, this);
	super::append(&content);

  super::append(&download);
	super::append(&cancel);

	downloadProgress.width = PANE_WIDTH;
	downloadProgress.position(SCREEN_WIDTH / 2 - downloadProgress.width / 2, PANE_WIDTH / 2 - 5);
	downloadProgress.color = 0xff0000ff;
	downloadProgress.dimBg = true;

  // download informations (not visible until the download is started)
	downloadStatus.position(SCREEN_WIDTH / 2 - downloadProgress.width / 2, PANE_WIDTH / 2 - 70 / SCALER);
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

void AppDetails::proceed()
{
	if (this->operating) return;

	this->operating = true;
	// event->update();

	// description of what we're doing
	super::append(&downloadProgress);
	super::append(&downloadStatus);

	// setup progress bar callback
	networking_callback = AppDetails::updateCurrentlyDisplayedPopup;
	libget_status_callback = AppDetails::updatePopupStatus;

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
}

void AppDetails::launch()
{
	if (!this->canLaunch) return;

#if defined(SWITCH)
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
#endif
}

void AppDetails::getSupported()
{
	Package* installer = get->lookup("NXthemes_Installer");
	if (installer != NULL)
		RootDisplay::switchSubscreen(new AppDetails(installer, appList));
}

void AppDetails::back()
{
	if (this->operating) return;

	RootDisplay::switchSubscreen(nullptr);
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
	if (event->isTouchDown())
		this->dragging = true;

	if (this->operating) return false;

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
	networking_callback = nullptr;
	libget_status_callback = nullptr;
}

void AppDetails::render(Element* parent)
{
	if (this->renderer == NULL)
		this->renderer = parent->renderer;
	if (this->parent == NULL)
		this->parent = parent;

	// draw white background
	CST_Rect dimens = { 0, 0, 920, PANE_WIDTH };

	CST_Color white = { 0xff, 0xff, 0xff, 0xff };

	CST_SetDrawColor(parent->renderer, white);
	CST_FillRect(parent->renderer, &dimens);

	CST_SetDrawColor(parent->renderer, white);

	// draw all elements
	super::render(parent);
}

int AppDetails::updatePopupStatus(int status, int num, int num_total)
{
	auto screen = RootDisplay::subscreen;
	std::stringstream statusText;

	if (screen != NULL)
	{
		AppDetails* popup = (AppDetails*)screen;
		Package* package = popup->package;

		if (status < 0 || status >= 5) return 0;
		const char* statuses[5] = { "Downloading ", "Installing ", "Removing ", "Reloading Metadata", "Syncing Packages"};

		statusText << statuses[status];

		if (status <= STATUS_REMOVING)
			statusText << package->title;

		statusText << "...";

		if (num_total != 1)
		{
			// num_total for this operation isn't 1, so let's display a counter in parens
			// (for instance, with multiple repos)
			statusText << " (" << num << "/" << num_total << ")";
		}

		popup->downloadStatus.setText(statusText.str());
		popup->downloadStatus.update();
	}
	return 0;
}

int AppDetails::updateCurrentlyDisplayedPopup(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	int now = CST_GetTicks();
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

	AppDetails::lastFrameTime = CST_GetTicks();

	return 0;
}

AppDetailsContent::AppDetailsContent(Package *package, bool useBannerIcons)
	: reportIssue("Report Issue", Y_BUTTON)
	, moreByAuthor("More by Author", X_BUTTON)
	, title(package->title.c_str(), 35, &black)
	, title2(package->author.c_str(), 27, &gray)
	, details(package->long_desc.c_str(), 20 / SCALER, &black, false, PANE_WIDTH + 20 / SCALER)
	, changelog((std::string("Changelog:\n") + package->changelog).c_str(), 20 / SCALER, &black, false, PANE_WIDTH + 20 / SCALER)
	, banner(useBannerIcons ? package->getBannerUrl().c_str() : package->getIconUrl().c_str(), [package]{
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
{
	title.position(MARGIN, 30);
	super::append(&title);

	reportIssue.position(920 - MARGIN - reportIssue.width, 45);
	super::append(&reportIssue);

	moreByAuthor.position(reportIssue.x - 20 - moreByAuthor.width, 45);
	super::append(&moreByAuthor);

	if (!useBannerIcons)
		banner.setScaleMode(SCALE_PROPORTIONAL_WITH_BG);

	banner.resize(767 / SCALER, 193 / SCALER);
	banner.position(BANNER_X / SCALER, BANNER_Y);
	super::append(&banner);

	title2.position(MARGIN, 80);
	super::append(&title2);

	// the main description (wrapped text)
	details.position((MARGIN / SCALER + 30), banner.y + banner.height + 22);
	super::append(&details);

	changelog.position((MARGIN / SCALER + 30), details.y + details.height + 30);
	super::append(&changelog);
}

void AppDetailsContent::render(Element* parent)
{
	if (this->parent == NULL)
		this->parent = parent;

	this->renderer = parent->renderer;

	ListElement::render(parent);
}

bool AppDetailsContent::process(InputEvents* event)
{
	bool ret = false;

	ret |= ListElement::processUpDown(event);

	return ret || ListElement::process(event);
}
