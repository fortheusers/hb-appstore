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
#include "ProgressScreen.hpp"
#include "ThemeManager.hpp"
#include "MainDisplay.hpp"
#include "main.hpp"

int AppDetails::lastFrameTime = 99;

AppDetails::AppDetails(Package& package, AppList* appList, AppCard* appCard)
	: package(std::make_shared<Package>(package))
	, get(appList->get)
	, appList(appList)
	, appCard(appCard)
{
	width = SCREEN_WIDTH;
	height = SCREEN_HEIGHT;
	
	rebuildUI();
}

void AppDetails::rebuildUI()
{
	removeAll();
	
	download = nullptr;
	cancel = nullptr;
	details = nullptr;
	content = nullptr;
	start = nullptr;
	errorText = nullptr;
	
	// set the BG to the theme's
	this->hasBackground = true;
	this->backgroundColor = fromCST(HBAS::ThemeManager::background);

	auto sidebarBg = std::make_unique<Element>();
	sidebarBg->hasBackground = true;
	sidebarBg->backgroundColor = RootDisplay::mainDisplay->backgroundColor;
	sidebarBg->position(SCREEN_WIDTH - 360, 0);
	sidebarBg->width = 360;
	sidebarBg->height = SCREEN_HEIGHT;
	addNode(std::move(sidebarBg));

	auto effectiveScale = getEffectiveScale();
	
	download = createNode<Button>(getAction(package.get()), package->getStatus() == INSTALLED ? X_BUTTON : A_BUTTON, true, 30 * effectiveScale);
	cancel = createNode<Button>(i18n("details.cancel"), B_BUTTON, true, 30 * effectiveScale, download->width);
	details = createNode<TextElement>(getPackageDetails(package.get()).c_str(), 20 * effectiveScale, &white, false, 300);
	content = createNode<AppDetailsContent>(package, appList->useBannerIcons);

	// download/update/remove button (2)

	download->position(SCREEN_WIDTH - 310, SCREEN_HEIGHT - 170);
	download->action = std::bind(&AppDetails::proceed, this);

	cancel->position(SCREEN_WIDTH - 310, SCREEN_HEIGHT - 90);
	cancel->action = std::bind(&AppDetails::back, this);

#if defined(_3DS) || defined(_3DS_MOCK)
	download->position(SCREEN_WIDTH * effectiveScale - download->width * effectiveScale, 360);
	cancel->position(SCREEN_WIDTH * effectiveScale - cancel->width * effectiveScale, 410);
#endif

	// display an additional launch/install button if the package is installed,  and has a binary or is a theme

	bool hasBinary = package->getBinary() != "none";
	bool isTheme = package->getCategory() == "theme";

	if (package->getStatus() != GET && (hasBinary || isTheme))
	{
		download->position(SCREEN_WIDTH - 310, SCREEN_HEIGHT - 250);
		cancel->position(SCREEN_WIDTH - 310, SCREEN_HEIGHT - 90);

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

		auto startBtn = std::make_unique<Button>(buttonLabel, START_BUTTON, true, 30, download->width);
		start = startBtn.get();
		start->position(SCREEN_WIDTH - 310, SCREEN_HEIGHT - 170);
		start->action = std::bind(&AppDetails::launch, this);
		super::addNode(std::move(startBtn));
	}
}	// more details

	details->position(SCREEN_WIDTH - 340, 50);

	// the scrollable portion of the app details page
	content->moreByAuthor->action = std::bind(&AppDetails::moreByAuthor, this);
	content->reportIssue->action = std::bind(&AppDetails::leaveFeedback, this);
}

AppDetails::~AppDetails()
{
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
				 << i18n("details.package") << " " << package->getPackageName() << "\n";

	auto downloadsCount = package->getDownloadCount();
	if (downloadsCount > 0)
	{
		// only show download count if it's not 0
		more_details << i18n("details.downloads") << " " << i18n_number(package->getDownloadCount()) << "\n";
	}
	more_details << i18n("details.updated") << " " << i18n_date(package->getUpdatedAtTimestamp()) << "\n\n"
				 << i18n("details.size") << " " << package->getHumanDownloadSize() << "\n";

	auto extractedSize = package->getExtractedSize();
	if (extractedSize > 0)
	{
		printf("Extracted size: %d\n", extractedSize);
		more_details << i18n("details.installsize") << " " << package->getHumanExtractedSize() << "\n";
	}
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

	// check for an existing partial download if installing/updating
	bool isInstalling = package->getStatus() != INSTALLED;
	if (isInstalling) {
		int partialPercent = package->hasPartialDownload(get->mTmp_path);
		// only show resume prompt if we have a meaningful amount downloaded (> 0%)
		if (partialPercent > 0) {
			showResumePrompt(partialPercent);
			return;
		}
	}

	// no partial download, proceed normally
	startInstallOrRemove(false);
}

void AppDetails::showResumePrompt(int percentComplete)
{
	std::vector<std::pair<std::string, std::string>> choices;
	
	std::stringstream resumeText;
	resumeText << i18n("details.resume") << " (" << percentComplete << "%)";
	choices.push_back({"resume", resumeText.str()});
	choices.push_back({"restart", i18n("details.restart")});
	
	auto resumePrompt = std::make_unique<DropDownChoices>(
		choices,
		[this](std::string choice) {			
			printf("DropDown callback invoked: choice=%s, this=%p, appList=%p\n", 
				choice.c_str(), this, this->appList);
			
			bool shouldResume = (choice == "resume");
			this->startInstallOrRemove(shouldResume);
		},
		true, // isDarkMode, TODO: match current theme
		i18n("details.resume.prompt") // Header text
	);
	
	RootDisplay::pushScreen(std::move(resumePrompt));
}

void AppDetails::startInstallOrRemove(bool resume)
{
	if (this->operating) return;

	this->operating = true;

	get->resetCancellation();

	printf("AppDetails::proceed: Creating and pushing ProgressScreen\n");
	std::string operation = package->getStatus() == INSTALLED ? i18n("details.remove.verb") : i18n("details.install.verb");
	
	// get partial download percentage if resuming
	double partialPercent = 0.0;
	if (resume) {
		int partial = package->hasPartialDownload(get->mTmp_path);
		if (partial > 0) {
			partialPercent = (double)partial;
		}
	}
	
	auto progressScreen = std::make_unique<ProgressScreen>(package, operation, partialPercent);
	
	progressScreen->setCancelCallback([this]() {
		this->operating = false;
		get->cancelCurrentOperation();
		printf("User cancelled operation\n");
	});
	
	RootDisplay::pushScreen(std::move(progressScreen));	
	printf("AppDetails::proceed: ProgressScreen pushed, topScreen = %p\n", RootDisplay::topScreen());

	// setup progress bar callback (signature depends on curl version)
	networking_callback = AppDetails::updateCurrentlyDisplayedPopup;
	networking_callback_data = (void*)get; // uses Get instance to check for cancellation
	libget_status_callback = AppDetails::updatePopupStatus;

	// if we're installing ourselves, we need to quit after on switch
	preInstallHook();

	// install or remove this package based on the package status
	if (this->package->getStatus() == INSTALLED)
		get->remove(*package);
	else
	{
		get->install(*package, resume);
		// save the icon to the SD card, for offline use
		// TODO: save directly from the texture cache instead of a new network call
		auto iconSavePath = std::string(get->mPkg_path) + "/" + package->getPackageName() + "/icon.png";
		
		std::string iconData;
		if (downloadFileToMemory(package->getIconUrl().c_str(), &iconData)) {
			std::ofstream iconFile(iconSavePath, std::ios::binary);
			if (iconFile.is_open()) {
				iconFile.write(iconData.c_str(), iconData.size());
				iconFile.close();
			}
		}
	}

	postInstallHook();

		// check if operation was cancelled, if so, user already popped ProgressScreen
	if (get->isCancelled()) {
		printf("Operation was cancelled, not popping screens\n");
		this->operating = false;
		return;
	}

	auto* appListPtr = this->appList;
	
	// pop two screens to dismiss progress + appdetails
	// (these calls are deferred, so appListPtr is still valid)
	RootDisplay::popScreen();
	RootDisplay::popScreen();
	
	// also, force update the main app list (also deferred)
	if (appListPtr) {
		RootDisplay::deferAction([appListPtr]() {
			printf("Updating app list after install/remove completion\n");
			appListPtr->update();
		});
	}
}

void AppDetails::launch()
{
	if (!this->canLaunch) return;

	char path[8 + strlen(package->getBinary().c_str())];

	snprintf(path, sizeof(path), ROOT_PATH "%s", package->getBinary().c_str() + 1);
	printf("Launch path: %s\n", path);

	FILE* file;
	bool successLaunch = false;

	if (package->getCategory() == "theme")
	{
		auto installer = get->lookup("NXthemes_Installer"); // This should probably be more dynamic in future, e.g. std::vector<Package*> Get::find_functionality("theme_installer")
		if (installer && installer->getStatus() != GET)
		{
			snprintf(path, sizeof(path), ROOT_PATH "%s", installer->getBinary().c_str() + 1);
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
		auto errText = std::make_unique<TextElement>(i18n("errors.applaunch"), 24, &red, false, 300);
	errorText = errText.get();
	errorText->position(970, 430);
	super::addNode(std::move(errText));
	this->canLaunch = false;
	}
}

void AppDetails::getSupported()
{
	auto installer = get->lookup("NXthemes_Installer");
	if (installer)
		RootDisplay::pushScreen(std::make_unique<AppDetails>(installer.value(), appList));
}

void AppDetails::back()
{
	if (this->operating) return;

	RootDisplay::popScreen();
}

void AppDetails::moreByAuthor()
{
	const char* author = this->package->getAuthor().c_str();
	appList->sidebar->searchQuery = std::string(author);
	appList->sidebar->curCategory = 0;
	appList->update();
	appList->y = 0;
	appList->keyboard->hidden = true;
	RootDisplay::popScreen();
}

void AppDetails::leaveFeedback()
{
	RootDisplay::pushScreen(std::make_unique<Feedback>(*(this->package)));
}

bool AppDetails::process(InputEvents* event)
{
	if (event->isTouchDown())
		this->dragging = true;

	if (this->operating) return false;

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

	if (!package->manifest.isValid())
	{
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
			themeArg += themePaths[i];
		}
		else
		{
			themeArg += themePaths[i];
			themeArg += ",";
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
	snprintf(args, sizeof(args), "%s %s", installerPath, themeArg.c_str() + 1);

	return this->launchFile(installerPath, args);
}

bool AppDetails::launchFile(char* path, char* context)
{
#if defined(SWITCH)
	// If setnexload works without problems, quit to make loader open next nro
	if (R_SUCCEEDED(envSetNextLoad(path, context)))
	{
		RootDisplay::mainDisplay->requestQuit();
		return true;
	}
#elif defined(__WIIU__)
	(void)context; // unused on wii u
	RPXLoaderStatus ret = RPXLoader_InitLibrary();
	if (ret == RPX_LOADER_RESULT_SUCCESS)
	{
		return RPXLoader_LaunchHomebrew(path) == RPX_LOADER_RESULT_SUCCESS;
	}
#else
	(void)context; // unused on other platforms
#endif
	printf("Would have launched %s, but not implemented on this platform\n", path);
	return false;
}

void AppDetails::postInstallHook()
{
	printf("postInstallHook: Clearing networking callbacks\n");
	networking_callback = nullptr;
	networking_callback_data = nullptr;
	libget_status_callback = nullptr;

	if (quitAfterInstall)
	{
		RootDisplay::mainDisplay->requestQuit();
	}
}

void AppDetails::render(Element* parent)
{
	if (this->parent == NULL)
		this->parent = parent;

	// draw all elements
	super::render(parent);
}

int AppDetails::updatePopupStatus(int status, int num, int num_total)
{
	// get the top screen which should be ProgressScreen during install/remove
	// TODO: verify this?
	ProgressScreen* progressScreen = dynamic_cast<ProgressScreen*>(RootDisplay::topScreen());
	
	if (progressScreen == nullptr)
		return 0;

	if (status < 0 || status >= 6)
		return 0;

	// hide cancel button when we're no longer in the downloading phase
	if (status != 0) {
		progressScreen->hideCancelButton();
	}

	std::string statuses[6] = {
		i18n("details.download.verb") + " ",
		i18n("details.install.verb") + " ",
		i18n("details.remove.verb") + " ",
		i18n("details.reloading"),
		i18n("details.syncing") + " ",
		i18n("details.analyzing") + " "
	};

	std::stringstream statusText;
	statusText << statuses[status] << "...";

	if (num_total != 1)
	{
		statusText << " (" << num << "/" << num_total << ")";
	}

	progressScreen->updateProgress(progressScreen->getProgress(), statusText.str());
	return 0;
}

int AppDetails::updateCurrentlyDisplayedPopup(void* clientp, double progress)
{
	// check if user requested cancellation
	Get* get = (Get*)clientp;
	if (get && get->isCancelled()) {
		printf("Cancellation requested, aborting download\n");
		return 1; // non-zero return cancels the transfer
	}

	int now = CST_GetTicks();
	int diff = now - AppDetails::lastFrameTime;

	// don't update the GUI too frequently here, it slows down downloading
	// (never return early if it's 100% done)
	if (diff < 32 && progress != 1.0)
		return 0;

	// get ProgressScreen from the top of the screen stack
	// TODO: see above TODO again
	ProgressScreen* progressScreen = dynamic_cast<ProgressScreen*>(RootDisplay::topScreen());
	
	if (progressScreen == NULL) {
		printf("updateCurrentlyDisplayedPopup: progressScreen is NULL (topScreen = %p)\n", RootDisplay::topScreen());
		return 0;
	}

	// printf("updateCurrentlyDisplayedPopup: Updating progress to %.2f%%\n", progress * 100);
	progressScreen->updateProgress(progress, "");

	// force a render/event process, as during the sync fetch, our callback is outside of the main loop
	MainDisplay* mainDisplay = (MainDisplay*)RootDisplay::mainDisplay;
	
	auto events = std::make_unique<InputEvents>();
	auto eventsPtr = events.get();
	while (events->update())
		mainDisplay->process(eventsPtr);
	mainDisplay->render(NULL);

	AppDetails::lastFrameTime = CST_GetTicks();

	return 0;
}
