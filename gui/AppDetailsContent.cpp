#include <fstream>
#include <sstream>

#if defined(SWITCH)
#include <switch.h>
#endif

#include "../libs/get/src/Get.hpp"
#include "../libs/get/src/Utils.hpp"

#include "../libs/chesto/src/RootDisplay.hpp"

#include "AppDetailsContent.hpp"
#include "Feedback.hpp"
#include "AppList.hpp"
#include "main.hpp"

std::string getTrimmedDetails(AppDetailsContent* content, std::string details)
{
  // first 350 characters, or first five lines, whichever first
  std::string trimmedDetails = details.substr(0, 350);
  int lineCount = 0;
  for (int i = 0; i < trimmedDetails.length(); i++)
  {
	if (trimmedDetails[i] == '\n')
	  lineCount++;
	if (lineCount >= 5)
	{
	  trimmedDetails = trimmedDetails.substr(0, i);
	  break;
	}
  }

  if (trimmedDetails.length() < details.length()) {
	trimmedDetails += "...";
  } else {
	// already expanded, so don't show the "read more" button
	// (or consider it already expanded, if ya will)
	content->expandedReadMore = true;
  }

  return trimmedDetails;
}

AppDetailsContent::AppDetailsContent(Package *package, bool useBannerIcons)
	: reportIssue("Report Issue", L_BUTTON)
	, moreByAuthor("More by Author", R_BUTTON)
	, title(package->getTitle().c_str(), 35, &black)
	, title2(package->getAuthor().c_str(), 27, &gray)
	, details("Package long description", 20 / SCALER, &black, false, PANE_WIDTH + 20 / SCALER)
	, changelog("If you're reading this text, something is wrong", 20 / SCALER, &black, false, PANE_WIDTH + 20 / SCALER)
	, showFiles("Show Installed Files List", ZL_BUTTON, false, 15)
	, showChangelog("Show Changelog", ZR_BUTTON, false, 15)
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
	, viewSSButton("Read More...", Y_BUTTON, false, 15)
{
	title.position(MARGIN, 30);
	super::append(&title);

	auto marginOffset = 920 - MARGIN;

	moreByAuthor.position(marginOffset - moreByAuthor.width, 45);
	marginOffset = moreByAuthor.x - 20;
	super::append(&moreByAuthor);

	if (package->getStatus() != GET) {
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
	details.setText(getTrimmedDetails(this, package->getLongDescription()).c_str());
	details.update();
	details.position((MARGIN / SCALER + 30), banner.y + banner.height + 22);
	super::append(&details);

	// "Read more..." and View screen shot button (reused/moves based on scroll (see process()))
	viewSSButton.position((banner.x + banner.width)/2 - viewSSButton.width/4, (details.y + details.height) + 20);

	// view file list button
	showFiles.position(banner.x, viewSSButton.y);
	showFiles.action = [this, package] {
		this->switchExtraInfo(package, extraContentState == SHOW_LIST_OF_FILES ? SHOW_NEITHER : SHOW_LIST_OF_FILES);
	};
	super::append(&showFiles);

	// view screen shots button
	showChangelog.position(banner.x + banner.width - showChangelog.width, viewSSButton.y);
	showChangelog.action = [this, package] {
		this->switchExtraInfo(package, extraContentState == SHOW_CHANGELOG ? SHOW_NEITHER : SHOW_CHANGELOG);
	};
	super::append(&showChangelog);

	changelog.position((MARGIN / SCALER + 30), showFiles.y + showFiles.height + 30);
	changelog.hidden = true;
	super::append(&changelog);

	if (useBannerIcons) {
		screenshotsContainer.position(BANNER_X / SCALER - 40, showFiles.y + showFiles.height + 15);

		// add screenshot netimages loaded from the package's screenshot urls
		// (package->screens is the count of screenshots)
		for (int i=0; i<package->getScreenshotCount(); i++) {
			auto ssUrl = package->getScreenShotUrl(i+1);
			NetImageElement* screenshot = new NetImageElement(ssUrl.c_str(), [package]{
				// if the screen shot fails to load, just use the app icon
				ImageElement* iconBackup = new ImageElement(RAMFS "res/gray_sq.png");
				return iconBackup;
			});
			screenshot->resize(820 / SCALER, 512 / SCALER);
			screenshot->setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
			screenshotsContainer.add(screenshot);
		}

		super::append(&screenshotsContainer);
	}

	viewSSButton.action = [this, package] {
		// open the current screen shot in a subscreen
		if (curScreenIdx == 0) {
			// read more button, probably (if not expanded)
			if (!expandedReadMore) {
				expandedReadMore = true;
				viewSSButton.hidden = true;
				int oldDetailsHeight = details.height + 80;
				details.setText(package->getLongDescription().c_str());
				details.update();

				// move the UI down by the new height of the expanded details
				this->slideUIDown(details.height - oldDetailsHeight + 80);
			}
		}
		else {
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

void AppDetailsContent::slideUIDown(int heightDiff) {
	showFiles.position(showFiles.x, showFiles.y + heightDiff);
	showChangelog.position(showChangelog.x, showChangelog.y + heightDiff);
	changelog.position(changelog.x, changelog.y + heightDiff);
	screenshotsContainer.position(screenshotsContainer.x, screenshotsContainer.y + heightDiff);
	viewSSButton.position(viewSSButton.x, viewSSButton.y + heightDiff);
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
			this->slideUIDown(heightDiff);
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
	// if the scroll isn't past half the screen height, don't set the curScreenshot
	if (!screenshotsContainer.hidden && (this->y + this->yOff < -SCREEN_HEIGHT/2)) {
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

	viewSSButton.hidden = false;
	if (curScreenshot == NULL) {
		// if readmore isn't pressed, put the button there
		if (expandedReadMore) {
			viewSSButton.hidden = true;
		} else {
			// no screenshot is on screen, so get our button out of there
			viewSSButton.position((banner.x + banner.width)/2 - viewSSButton.width/4, (details.y + details.height + 20));
			if (viewSSButton.getText() != "Read More...") {
				viewSSButton.updateText("Read More...");
			}
			curScreenIdx = 0;
		}
	} else {
		// if we have a screenshot on screen, let's update the View button's position
		if (viewSSButton.getText() != "View") {
			viewSSButton.updateText("View");
		}
		viewSSButton.position(
			screenshotsContainer.x + curScreenshot->x + curScreenshot->width - viewSSButton.width - 5,
			screenshotsContainer.y + curScreenshot->y + curScreenshot->height - viewSSButton.height - 5
		);
		// std::cout << "viewSSButton position: " << viewSSButton.x << ", " << viewSSButton.y << std::endl;
	}

	return ret || ListElement::process(event);
}

void AppDetailsContent::switchExtraInfo(Package* package, int newState) {

	// update button text
	showFiles.updateText((std::string(newState == SHOW_LIST_OF_FILES ? "Hide" : "Show") + " Installed Files List").c_str());
	showChangelog.updateText((std::string(newState == SHOW_CHANGELOG ? "Hide" : "Show") + " Changelog").c_str());

	// hide/show changelog text based on if neither is true
	changelog.hidden = newState == SHOW_NEITHER;
	extraContentState = newState;

	// update the "changelog" text depending on which action we're doing
	if (newState == SHOW_CHANGELOG)
	{
		changelog.setText(std::string("Changelog:\n") + package->getChangelog().c_str());
		changelog.setFont(NORMAL);
		changelog.update();
	}
	else if (newState == SHOW_LIST_OF_FILES)
	{
		std::stringstream allEntries;

		// if it's an installed package, use the already installed manifest
		// (LOCAL -> UPDATE packages won't have a manifest)
		auto status = package->getStatus();
		if ((status == INSTALLED || status == UPDATE) && package->manifest .isValid()) {
			allEntries << "Currently Installed Files:\n";
			for (auto &entry : package->manifest.getEntries()) {
				allEntries << entry.raw << "\n";
			}
			allEntries << "\n";
		}

		if (status != INSTALLED) {
			// manifest is either non-local, or we need to display both, download it from the server
			std::string data("");
			downloadFileToMemory(package->getManifestUrl().c_str(), &data);
			allEntries << "Manifest of Remote Files:\n" << data;
		}

		changelog.setText(std::string("") + allEntries.str().c_str());
		changelog.setFont(MONOSPACED);
		changelog.update();
	}

	// update the position of the screenshots based on text
	screenshotsContainer.position(
		screenshotsContainer.x,
		showFiles.y + showFiles.height + 30 + (changelog.hidden ? 0 : changelog.height)
	);
}