#include <fstream>
#include <sstream>

#if defined(SWITCH)
#include <switch.h>
#endif

#if defined(__WIIU__)
#include "../libs/librpxloader/include/rpxloader/rpxloader.h"
#endif

#include "../libs/get/src/Get.hpp"
#include "../libs/get/src/Utils.hpp"

#include "../libs/chesto/src/RootDisplay.hpp"

#include "AppDetails.hpp"
#include "AppList.hpp"
#include "Feedback.hpp"
#include "ThemeManager.hpp"
#include "main.hpp"

int AppDetails::lastFrameTime = 99;

AppDetails::AppDetails(Package& package, AppList* appList, AppCard* appCard)
	: package(&package)
	, get(appList->get)
	, appList(appList)
	, appCard(appCard)
	, downloadProgress()
	, download(getAction(&package), package.getStatus() == INSTALLED ? X_BUTTON : A_BUTTON, true, 30 / SCALER)
	, cancel(i18n("details.cancel"), B_BUTTON, true, 30 / SCALER, download.width)
	, details(getPackageDetails(&package).c_str(), 20, &white, false, 300)
	, content(&package, appList->useBannerIcons)
	, downloadStatus(i18n("details.status"), 30 / SCALER, &white)
{
	// TODO: show current app status somewhere

	// download/update/remove button (2)

	download.position(SCREEN_WIDTH - 310, SCREEN_HEIGHT - 170);
	download.action = std::bind(&AppDetails::proceed, this);

	cancel.position(SCREEN_WIDTH - 310, SCREEN_HEIGHT - 90);
	cancel.action = std::bind(&AppDetails::back, this);

#if defined(_3DS) || defined(_3DS_MOCK)
	download.position(SCREEN_WIDTH / 2 - download.width / 2, 360);
	cancel.position(SCREEN_WIDTH / 2 - cancel.width / 2, 410);
#endif

	// display an additional launch/install button if the package is installed,  and has a binary or is a theme

	bool hasBinary = package.getBinary() != "none";
	bool isTheme = package.getCategory() == "theme";

	if (package.getStatus() != GET && (hasBinary || isTheme))
	{
		download.position(SCREEN_WIDTH - 310, SCREEN_HEIGHT - 250);
		cancel.position(SCREEN_WIDTH - 310, SCREEN_HEIGHT - 90);

		std::string buttonLabel = i18n("details.launch");
		bool injectorPresent = false;

		if (isTheme) // should only happen on switch
		{
			auto installer = get->lookup("NXthemes_Installer");
			injectorPresent = installer ? true : false; // whether or not the currently hardcoded installer package exists, in the future becomes something functionality-based like "theme_installer"
			buttonLabel = (injectorPresent && installer->getStatus() == GET) ? i18n("details.injector") : i18n("details.inject");
		}

		// show the third button if a binary is present, or a theme injector is available (installed or not)
		if (hasBinary || injectorPresent)
		{
			this->canLaunch = true;

			start = new Button(buttonLabel, START_BUTTON, true, 30, download.width);
			start->position(SCREEN_WIDTH - 310, SCREEN_HEIGHT - 170);
			start->action = std::bind(&AppDetails::launch, this);
			super::append(start);
		}
	}

	// more details

	details.position(SCREEN_WIDTH - 340, 50);
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
}

std::string AppDetails::getPackageDetails(Package* package)
{
	// lots of details that we know about the package
	std::stringstream more_details;
	more_details << i18n("details.title") << " " << package->getTitle() << "\n"
				 << package->getShortDescription() << "\n\n"
				 << i18n("details.author") << " " << package->getAuthor() << "\n"
				 << i18n("details.version") << " " << package->getVersion() << "\n"
				 << i18n("details.license") << " " << package->getLicense() << "\n\n"
				 << i18n("details.package") << " " << package->getPackageName() << "\n"
				 << i18n("details.downloads") << " " << i18n_number(package->getDownloadCount()) << "\n"
				 << i18n("details.updated") << " " << i18n_date(package->getUpdatedAtTimestamp())<< "\n\n"
				 << i18n("details.size") << " " << package->getHumanDownloadSize() << "\n"
				 << i18n("details.installsize") << " " << package->getHumanExtractedSize() << "\n";
	return more_details.str();
}

std::string AppDetails::getAction(Package* package)
{
	switch (package->getStatus())
	{
	case GET:
		return i18n("details.download");
	case UPDATE:
		return i18n("details.update");
	case INSTALLED:
		return i18n("details.remove");
	case LOCAL:
		return i18n("details.reinstall");
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
	if (this->package->getStatus() == INSTALLED)
		get->remove(*package);
	else {
		get->install(*package);
		// save the icon to the SD card, for offline use
		if (appCard != NULL) {
			auto iconSavePath = std::string(get->mPkg_path) + "/" + package->getPackageName() + "/icon.png";
			appCard->icon.saveTo(iconSavePath);
			//TODO: load from a cache instead!!
		}
	}

	postInstallHook();

	// refresh the screen
	RootDisplay::switchSubscreen(nullptr);

	this->operating = false;
	this->appList->update();
}

void AppDetails::launch()
{
	if (!this->canLaunch) return;

	char path[8 + strlen(package->getBinary().c_str())];

	snprintf(path, sizeof(path), ROOT_PATH "%s", package->getBinary().c_str()+1);
	printf("Launch path: %s\n", path);

	FILE* file;
	bool successLaunch = false;

	if (package->getCategory() == "theme")
	{
		auto installer = get->lookup("NXthemes_Installer"); // This should probably be more dynamic in future, e.g. std::vector<Package*> Get::find_functionality("theme_installer")
		if (installer && installer->getStatus() != GET)
		{
			snprintf(path, sizeof(path), ROOT_PATH "%s", installer->getBinary().c_str()+1);
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
		// Final check if path actually exists
		if ((file = fopen(path, "r")))
		{
			fclose(file);
			printf("Path OK, Launching...\n");
			successLaunch = this->launchFile(path, path);
		}
		else
			successLaunch = false;
	}

	if (!successLaunch)
	{
		// printf("Failed to launch.");
		errorText = new TextElement(i18n("errors.applaunch"), 24, &red, false, 300);
		errorText->position(970, 430);
		super::append(errorText);
		this->canLaunch = false;
	}

}

void AppDetails::getSupported()
{
	auto installer = get->lookup("NXthemes_Installer");
	if (installer)
		RootDisplay::switchSubscreen(new AppDetails(installer.value(), appList));
}

void AppDetails::back()
{
	if (this->operating) return;

	RootDisplay::switchSubscreen(nullptr);
}

void AppDetails::moreByAuthor()
{
	const char* author = this->package->getAuthor().c_str();
	appList->sidebar->searchQuery = std::string(author);
	appList->sidebar->curCategory = 0;
	appList->update();
	appList->y = 0;
	appList->keyboard.hidden = true;
	RootDisplay::switchSubscreen(nullptr);
}

void AppDetails::leaveFeedback()
{
	RootDisplay::switchSubscreen(new Feedback(*(this->package)));
}

bool AppDetails::process(InputEvents* event)
{
	if (event->isTouchDown())
		this->dragging = true;

	if (this->operating) return false;

	if (content.showingScreenshot)
	{
		// if the screenshot is displayed, it's kind of like a second subscreen, and eats all inputs
		// TODO: this is a pattern chesto should handle better (like a stack of subscreens)
		return elements[elements.size() - 1]->process(event);
	}
	return super::process(event);
}

void AppDetails::preInstallHook()
{
// if on wii u and installing ourselves, we need to unmount our WUHB and exit after
#if defined(__WIIU__)
	if (this->package->getPackageName() == APP_SHORTNAME)
	{
		RPXLoaderStatus ret = RPXLoader_InitLibrary();
		if (ret == RPX_LOADER_RESULT_SUCCESS)
		{
			// unmount ourselves
			RPXLoader_UnmountCurrentRunningBundle();
		}
	}
#endif
}

bool AppDetails::themeInstall(char* installerPath)
{
	std::string ManifestPathInternal = "manifest.install";
	std::string ManifestPath = get->mPkg_path + this->package->getPackageName() + "/" + ManifestPathInternal;

	std::vector<std::string> themePaths;

	if (!package->manifest.isValid()) {
		package->manifest = Manifest(ManifestPath, ROOT_PATH);
	}

	if (package->manifest.isValid())
	{
		auto entries = package->manifest.getEntries();
		for (size_t i = 0; i <= entries.size() - 1; i++)
		{
			if (entries[i].operation == MUPDATE && entries[i].extension == "nxtheme")
			{
				printf("Found nxtheme\n");
				themePaths.push_back(entries[i].path);
			}
		}
	}
	else
	{
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
	snprintf(args, sizeof(args), "%s %s", installerPath, themeArg.c_str()+1);

	return this->launchFile(installerPath, args);
}

bool AppDetails::launchFile(char* path, char* context)
{
#if defined(SWITCH)
	// If setnexload works without problems, quit to make loader open next nro
	if (R_SUCCEEDED(envSetNextLoad(path, context)))
	{
		quit();
		return true;
	}
#elif defined(__WIIU__)
	RPXLoaderStatus ret = RPXLoader_InitLibrary();
	if (ret == RPX_LOADER_RESULT_SUCCESS)
	{
		return RPXLoader_LaunchHomebrew(path) == RPX_LOADER_RESULT_SUCCESS;
	}
#endif
	printf("Would have launched %s, but not implemented on this platform\n", path);
	return false;
}

void AppDetails::postInstallHook()
{
	networking_callback = nullptr;
	libget_status_callback = nullptr;

	if (quitAfterInstall)
	{
		RootDisplay::mainDisplay->events->quitaction();
	}
}

void AppDetails::render(Element* parent)
{
	if (this->parent == NULL)
		this->parent = parent;

	// draw white background
	CST_Rect dimens = { 0, 0, SCREEN_WIDTH - 360, SCREEN_HEIGHT };

	CST_SetDrawColor(RootDisplay::renderer, HBAS::ThemeManager::background);
	CST_FillRect(RootDisplay::renderer, &dimens);

	CST_SetDrawColor(RootDisplay::renderer, HBAS::ThemeManager::background);

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
		std::string statuses[6] = {
			i18n("details.download.verb") + " ",
			i18n("details.install.verb") + " ",
			i18n("details.remove.verb") + " ",
			i18n("details.reloading"),
			i18n("details.syncing") + " ",
			i18n("details.analyzing") + " "
		};

		statusText << statuses[status];

		if (status <= STATUS_REMOVING)
			statusText << package->getTitle();

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
				RootDisplay::mainDisplay->process(events);
			RootDisplay::mainDisplay->render(NULL);
		}
	}

	AppDetails::lastFrameTime = CST_GetTicks();

	return 0;
}
