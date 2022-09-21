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
#include "ImageCache.hpp"
#include "main.hpp"

AppDetailsContent::AppDetailsContent(Package *package, bool useBannerIcons)
	: reportIssue("Report Issue", L_BUTTON)
	, moreByAuthor("More by Author", R_BUTTON)
	, title(package->title.c_str(), 35, &black)
	, title2(package->author.c_str(), 27, &gray)
	, details(package->long_desc.c_str(), 20 / SCALER, &black, false, PANE_WIDTH + 20 / SCALER)
	, changelog((std::string("Changelog:\n") + package->changelog).c_str(), 20 / SCALER, &black, false, PANE_WIDTH + 20 / SCALER)
	, showFiles("Show Installed Files List", ZL_BUTTON)
	, showScreenshots("Show Changelog", ZR_BUTTON)
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