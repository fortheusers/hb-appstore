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
	, download(getAction(package), package->status == INSTALLED ? X_BUTTON : A_BUTTON, true, 30 / SCALER)
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

	if (content.showingScreenshot) {
		// if the screenshot is displayed, it's kind of like a second subscreen, and eats all inputs
		// TODO: this is a pattern chesto should handle better (like a stack of subscreens)
		return elements[elements.size()-1]->process(event);
	}
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
	if (this->parent == NULL)
		this->parent = parent;

	// draw white background
	CST_Rect dimens = { 0, 0, 920, PANE_WIDTH };

	CST_Color white = { 0xff, 0xff, 0xff, 0xff };

	CST_SetDrawColor(RootDisplay::renderer, white);
	CST_FillRect(RootDisplay::renderer, &dimens);

	CST_SetDrawColor(RootDisplay::renderer, white);

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
		const char* statuses[6] = { "Downloading ", "Installing ", "Removing ", "Reloading Metadata", "Syncing Packages", "Analyzing Files" };

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
	: reportIssue("Report Issue", L_BUTTON)
	, moreByAuthor("More by Author", R_BUTTON)
	, title(package->title.c_str(), 35, &black)
	, title2(package->author.c_str(), 27, &gray)
	, details(package->long_desc.c_str(), 20 / SCALER, &black, false, PANE_WIDTH + 20 / SCALER)
	, changelog((std::string("Changelog:\n") + package->changelog).c_str(), 20 / SCALER, &black, false, PANE_WIDTH + 20 / SCALER)
	, showFiles("Show Installed Files List", ZL_BUTTON)
	, showScreenshots("Show Screenshots", ZR_BUTTON)
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
	, screenshotsContainer(COL_LAYOUT, 20)
	, viewSSButton("View", Y_BUTTON, false, 15)
{
	title.position(MARGIN, 30);
	super::append(&title);

	auto marginOffset = 920 - MARGIN;

	moreByAuthor.position(marginOffset - moreByAuthor.width, 45);
	marginOffset = moreByAuthor.x - 20;
	super::append(&moreByAuthor);

	if (package->status != GET) {
		reportIssue.position(marginOffset - reportIssue.width, 45);
		super::append(&reportIssue);
	}

	banner.setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
	banner.resize(767 / SCALER, 93 / SCALER);	// banners use icon height now
	banner.position(BANNER_X / SCALER, BANNER_Y);
	super::append(&banner);

	title2.position(MARGIN, 80);
	super::append(&title2);

	// the main description (wrapped text)
	details.position((MARGIN / SCALER + 30), banner.y + banner.height + 22);
	super::append(&details);

	// view file list button
	showFiles.position((MARGIN / SCALER + 30), details.y + details.height + 30);
	showFiles.action = [this, package] {
		std::stringstream allEntries;

		if (showingFilesList) {
			showScreenshots.hidden = false;
			showFiles.updateText("Show Installed Files List");
			changelog.setText((std::string("Changelog:\n") + package->changelog).c_str());
			changelog.setFont(NORMAL);
		} else {
			showScreenshots.hidden = true;
			showFiles.updateText("Hide Installed Files List");

			// if it's an installed package, use the already installed manifest
			// (LOCAL -> UPDATE packages won't have a manifest)
			if ((package->status == INSTALLED || package->status == UPDATE) && package->manifest != NULL) {
				allEntries << "Currently Installed Files:\n";
				for (auto &entry : package->manifest->entries) {
					allEntries << entry.raw << "\n";
				}
				allEntries << "\n";
			}

			if (package->status != INSTALLED) {
				// manifest is either non-local, or we need to display both
				std::string data("");
				downloadFileToMemory(package->getManifestUrl().c_str(), &data);
				allEntries << "Manifest of Remote Files:\n" << data;
			}

			changelog.setText(std::string("") + allEntries.str().c_str());
			changelog.setFont(MONOSPACED);
		}
		changelog.update();
		showingFilesList = !showingFilesList;
	};
	super::append(&showFiles);

	// view screen shots button
	showScreenshots.position((showFiles.x + 15) + showFiles.width, details.y + details.height + 30);
	showScreenshots.action = [this, package] {
		if (showingScreenshots) {
			showFiles.hidden = false;
			showScreenshots.updateText("Show Screenshots");
			changelog.hidden = false;
			showScreenshots.position((showFiles.x + 15) + showFiles.width, details.y + details.height + 30);
		} else {
			showFiles.hidden = true;
			showScreenshots.updateText("Hide Screenshots");
			changelog.hidden = true;
			showScreenshots.position(showFiles.x, details.y + details.height + 30);
		}
		showingScreenshots = !showingScreenshots;
	};
	super::append(&showScreenshots);

	// changelog.position((MARGIN / SCALER + 30), showFiles.y + showFiles.height + 30);
	// super::append(&changelog);

	if (useBannerIcons) {
		screenshotsContainer.position(BANNER_X / SCALER - 20, showFiles.y + showFiles.height + 30);

		// add screenshot netimages loaded from the package's screenshot urls
		// (package->screens is the count of screenshots)
		for (int i=0; i<package->screens; i++) {
			auto ssUrl = package->getScreenShotUrl(i+1);
			NetImageElement* screenshot = new NetImageElement(ssUrl.c_str(), [package]{
				// if the screen shot fails to load, just use the app icon
				ImageElement* iconBackup = new ImageElement(RAMFS "res/gray_sq.png");
				return iconBackup;
			});
			screenshot->resize(767 / SCALER, 479 / SCALER);
			screenshot->setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
			screenshotsContainer.add(screenshot);
		}

		super::append(&screenshotsContainer);

		viewSSButton.action = [this, package] {
			// open the current screen shot in a subscreen
			if (curScreenIdx != 0) {
				Element* subParent = new Element();
				auto ssUrl = package->getScreenShotUrl(curScreenIdx);
				NetImageElement* screenshot = new NetImageElement(ssUrl.c_str(), [package]{
					// if the screen shot fails to load, just use the app icon
					ImageElement* iconBackup = new ImageElement(RAMFS "res/gray_sq.png");
					return iconBackup;
				});
				screenshot->resize(SCREEN_WIDTH, SCREEN_HEIGHT);
				screenshot->setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
				screenshot->position(0, 0);
				subParent->child(screenshot);

				Button* dismiss = new Button("Back", B_BUTTON, false, 15);
				auto appDetails = RootDisplay::subscreen;
				dismiss->action = [this, appDetails, subParent] {
					appDetails->remove(subParent);
					this->showingScreenshot = false;
					delete subParent;
				};
				dismiss->position(5, SCREEN_HEIGHT - dismiss->height - 5);
				subParent->child(dismiss);

				this->showingScreenshot = true;
				appDetails->child(subParent);
			}

		};
		super::append(&viewSSButton);
	}
}

void AppDetailsContent::render(Element* parent)
{
	if (this->parent == NULL)
		this->parent = parent;
	
	// check if the banner needs to be resized
	if (banner.loaded) {
		auto prevBannerHeight = banner.height;
		// a banner icon loaded, so use banner icon height
		banner.resize(767 / SCALER, 193 / SCALER);

		auto heightDiff = banner.height - prevBannerHeight;
		if (heightDiff > 0) {
			// banner height increased, so move everything down
			details.position(details.x, details.y + heightDiff);
			showFiles.position(showFiles.x, showFiles.y + heightDiff);
			showScreenshots.position(showScreenshots.x, showScreenshots.y + heightDiff);
			changelog.position(changelog.x, changelog.y + heightDiff);
			screenshotsContainer.position(screenshotsContainer.x, screenshotsContainer.y + heightDiff);
		}
	}

	ListElement::render(parent);
}

bool AppDetailsContent::process(InputEvents* event)
{
	bool ret = false;
	if (showingScreenshot) {
		// ignore all input events while showing a screenshot
		return ret;
	}

	ret |= ListElement::processUpDown(event);

	Element* curScreenshot = NULL;

	// get the screenshot that is "most" on screen
	if (!screenshotsContainer.hidden) {
		int count = 0;
		for (auto image : screenshotsContainer.elements) {
			count++;
			CST_Rect rect = {
				this->x + this->xOff + screenshotsContainer.x + xOff + image->x,
				this->y + this->yOff + screenshotsContainer.y + image->yOff + image->y - image->height,
				image->width,
				image->height*1.6
			};
			// std::cout << "rect: " << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << std::endl;
			if (!CST_isRectOffscreen(&rect)) {
				// it's on screen, let's go with it
				curScreenshot = image;
				curScreenIdx = count;
				break;
			}
		}
	}

	if (curScreenshot != NULL) {
		// if we have a screenshot on screen, let's update the View button's position
		viewSSButton.position(
			screenshotsContainer.x + curScreenshot->x + curScreenshot->width - viewSSButton.width - 5,
			screenshotsContainer.y + curScreenshot->y + curScreenshot->height - viewSSButton.height - 5
		);
		// std::cout << "viewSSButton position: " << viewSSButton.x << ", " << viewSSButton.y << std::endl;
	}

	return ret || ListElement::process(event);
}
