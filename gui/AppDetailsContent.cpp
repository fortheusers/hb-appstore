#include <algorithm>
#include <fstream>
#include <sstream>

#if defined(SWITCH)
#include <switch.h>
#endif

#include "../libs/get/src/Get.hpp"
#include "../libs/get/src/Utils.hpp"

#include "../libs/chesto/src/RootDisplay.hpp"
#include "../libs/chesto/src/Screen.hpp"

#include "AppDetailsContent.hpp"
#include "AppList.hpp"
#include "Feedback.hpp"
#include "MainDisplay.hpp"
#include "ThemeManager.hpp"
#include "main.hpp"


// fullscreen screenshots now use a full Screen
class ScreenshotScreen : public Screen {
public:
	ScreenshotScreen(std::string ssUrl, AppDetailsContent* content)
		: content(content)
	{
		if (!ssUrl.empty()) {
			auto screenshot = std::make_unique<NetImageElement>(ssUrl.c_str(), []
				{
				// if the screen shot fails to load, just use the app icon
		ImageElement* iconBackup = new ImageElement(RAMFS "res/gray_sq.png");
		return iconBackup; });
		screenshot->resize(SCREEN_WIDTH, SCREEN_HEIGHT);
		screenshot->setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
		screenshot->position(0, 0);
		this->addNode(std::move(screenshot));
	} else {
		// No valid repo - show placeholder
		auto placeholder = std::make_unique<ImageElement>(RAMFS "res/gray_sq.png");
		placeholder->resize(SCREEN_WIDTH, SCREEN_HEIGHT);
		placeholder->setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
		placeholder->position(0, 0);
		this->addNode(std::move(placeholder));
	}

	AppDetailsContent* contentPtr = this->content;
	auto dismiss = std::make_unique<Button>(i18n("details.back"), B_BUTTON, false, 15);
	dismiss->action = [contentPtr]
	{
		contentPtr->showingScreenshot = false;
		RootDisplay::popScreen();
	};
	dismiss->position(5, SCREEN_HEIGHT - dismiss->height - 5);
	this->addNode(std::move(dismiss));
}
void rebuildUI() override {
	}
	
private:
	AppDetailsContent* content;
};

std::string getTrimmedDetails(AppDetailsContent* content, std::string details)
{
	// first 350 characters, or first five lines, whichever first
	std::string trimmedDetails = details.substr(0, 350);
	int lineCount = 0;
	for (size_t i = 0; i < trimmedDetails.length(); i++)
	{
		if (trimmedDetails[i] == '\n')
			lineCount++;
		if (lineCount >= 5)
		{
			trimmedDetails = trimmedDetails.substr(0, i);
			break;
		}
	}

	if (trimmedDetails.length() < details.length())
	{
		trimmedDetails += "...";
	}
	else
	{
		// already expanded, so don't show the "read more" button
		// (or consider it already expanded, if ya will)
		content->expandedReadMore = true;
	}

	return trimmedDetails;
}

AppDetailsContent::AppDetailsContent(std::shared_ptr<Package> package, bool useBannerIcons)
{
	auto effectiveScale = getEffectiveScale();

	reportIssue = createNode<Button>(i18n("contents.report"), L_BUTTON, isDark, 15);
	moreByAuthor = createNode<Button>(i18n("contents.more"), R_BUTTON, isDark, 15);
	title = createNode<TextElement>(package->getTitle().c_str(), 35 * effectiveScale, &HBAS::ThemeManager::textPrimary);
	title2 = createNode<TextElement>(package->getAuthor().c_str(), 27 * effectiveScale, &HBAS::ThemeManager::textSecondary);
	details = createNode<TextElement>(i18n("contents.placeholder1"), 20 * effectiveScale, &HBAS::ThemeManager::textPrimary, false, PANE_WIDTH + 20 * effectiveScale);
	changelog = createNode<TextElement>(i18n("contents.placeholder2"), 20 * effectiveScale, &HBAS::ThemeManager::textPrimary, false, PANE_WIDTH + 20 * effectiveScale);
	showFiles = createNode<DropDown>(ZL_BUTTON, std::vector<std::pair<std::string, std::string>> {
																 { "N/A", "N/A" },
															 },
		[](std::string choice)
		{
        // do nothing, this is read only innfo
		std::cout << "Selected choice: " << choice << std::endl; }, 15, i18n("contents.showinstalled"), isDark);
	showChangelog = createNode<Button>(i18n("contents.showchangelog"), ZR_BUTTON, isDark, 15);
	banner = createNode<NetImageElement>(useBannerIcons ? package->getBannerUrl().c_str() : package->getIconUrl().c_str(), [package]
		{
			// If the banner fails to load, use an icon banner
			NetImageElement* icon = new NetImageElement(package->getIconUrl().c_str(), []{
				// if even the icon fails to load, use the default icon
				ImageElement *defaultIcon = new ImageElement(RAMFS "res/default.png");
				defaultIcon->setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
				return defaultIcon;
			});
			icon->setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
			return icon; });
	screenshotsContainer = createNode<Container>(COL_LAYOUT, 20);
	viewSSButton = createNode<Button>(i18n("contents.readmore"), Y_BUTTON, isDark, 15);

	title->position(MARGIN, 30);

	auto marginOffset = 920 - MARGIN;

	moreByAuthor->position(marginOffset - moreByAuthor->width, 45);
	marginOffset = moreByAuthor->x - 20;

	if (package->getStatus() != GET)
	{
		reportIssue->position(marginOffset - reportIssue->width, 45);
	}
	else
	{
		reportIssue->hide();
	}

	banner->setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
	banner->resize(848 * effectiveScale, 208 * effectiveScale); // banners use icon height now
	banner->position(BANNER_X * effectiveScale - 26, BANNER_Y);

	title2->position(MARGIN, 80);

	// the main description (wrapped text)
	details->setText(getTrimmedDetails(this, package->getLongDescription()).c_str());
	details->update();
	details->position((MARGIN * effectiveScale + 30), banner->y + banner->height + 22);

	// "Read more..." and View screen shot button (reused/moves based on scroll (see process()))
	viewSSButton->position((banner->x + banner->width) / 2 - viewSSButton->width / 4, (details->y + details->height) + 20);

	// view file list dropdown button
	showFiles->position(banner->x, viewSSButton->y);
	auto showDropdownFunc = showFiles->action;
	showFiles->action = [this, package, showDropdownFunc]
	{
		// only populate choices if not already populated (avoids re-downloading)
		if (showFiles->choices.size() <= 1) {
			showFiles->choices = getManifestFiles(package);
		}
		showDropdownFunc();
	};

	// view screen shots button
	showChangelog->position(banner->x + banner->width - showChangelog->width, viewSSButton->y);
	showChangelog->action = [this, package]
	{
		this->switchExtraInfo(package, extraContentState == SHOW_CHANGELOG ? SHOW_NEITHER : SHOW_CHANGELOG);
	};

	changelog->position((MARGIN * effectiveScale + 30), showFiles->y + showFiles->height + 30);
	changelog->hidden = true;

	if (useBannerIcons)
	{
		screenshotsContainer->position(BANNER_X * effectiveScale - 40, showFiles->y + showFiles->height + 15);

		// add screenshot netimages loaded from the package's screenshot urls
		// (package->screens is the count of screenshots)
		for (int i = 0; i < package->getScreenshotCount(); i++)
		{
			auto ssUrl = package->getScreenShotUrl(i + 1);
			if (ssUrl.empty()) {
				continue;
			}
			auto screenshot = std::make_unique<NetImageElement>(ssUrl.c_str(), [package]
				{
				// if the screen shot fails to load, just use the app icon
				ImageElement* iconBackup = new ImageElement(RAMFS "res/gray_sq.png");
				return iconBackup; });
			screenshot->resize(820 * effectiveScale, 512 * effectiveScale);
			screenshot->setScaleMode(SCALE_PROPORTIONAL_WITH_BG);
			screenshotsContainer->add(std::move(screenshot));
		}
	}

	viewSSButton->action = [this, package]
	{
		// open the current screen shot in a subscreen
		if (curScreenIdx == 0)
		{
			// read more button, probably (if not expanded)
			if (!expandedReadMore)
			{
				expandedReadMore = true;
				viewSSButton->hidden = true;
				int oldDetailsHeight = details->height + 80;
				details->setText(package->getLongDescription().c_str());
				details->update();

				// move the UI down by the new height of the expanded details
				this->slideUIDown(details->height - oldDetailsHeight + 80);
			}
		}
		else
		{

			this->showingScreenshot = true;

			std::string ssUrl = package->getScreenShotUrl(curScreenIdx);
			RootDisplay::pushScreen(std::make_unique<ScreenshotScreen>(ssUrl, this));
		}
	};
}

void AppDetailsContent::slideUIDown(int heightDiff)
{
	showFiles->position(showFiles->x, showFiles->y + heightDiff);
	showChangelog->position(showChangelog->x, showChangelog->y + heightDiff);
	changelog->position(changelog->x, changelog->y + heightDiff);
	screenshotsContainer->position(screenshotsContainer->x, screenshotsContainer->y + heightDiff);
	viewSSButton->position(viewSSButton->x, viewSSButton->y + heightDiff);
}

void AppDetailsContent::render(Element* parent)
{
	if (this->parent == NULL)
		this->parent = parent;

	// check if the banner needs to be resized
	if (banner->loaded)
	{
		auto prevBannerHeight = banner->height;
		auto effectiveScale = getEffectiveScale();
		// a banner icon loaded, so use banner icon height
		banner->resize(848 * effectiveScale, 208 * effectiveScale);

		auto heightDiff = banner->height - prevBannerHeight;
		if (heightDiff > 0)
		{
			// banner height increased, so move everything down
			details->position(details->x, details->y + heightDiff);
			this->slideUIDown(heightDiff);
		}
	}

	ListElement::render(parent);
}

bool AppDetailsContent::process(InputEvents* event)
{
	bool ret = false;
	if (showingScreenshot)
	{
		// ignore all input events while showing a screenshot
		return ret;
	}

	ret |= ListElement::processUpDown(event);

	Element* curScreenshot = NULL;

	// get the screenshot that is "most" on screen
	// if the scroll isn't past half the screen height, don't set the curScreenshot
	if (!screenshotsContainer->hidden && (this->y < -SCREEN_HEIGHT / 2))
	{
		int count = 0;
		for (auto& imagePtr : screenshotsContainer->elements)
		{
			count++;
			Element* image = imagePtr.get();
			int rectHeight = (image->height * 8) / 5;
			
			// uses absolute positions (xAbs/yAbs) for rect bounds checking
			CST_Rect rect = {
				image->xAbs,
				image->yAbs - image->height,
				image->width,
				rectHeight
			};

			// std::cout << "rect: " << rect.x << ", " << rect.y << ", " << rect.w << ", " << rect.h << std::endl;
			if (!CST_isRectOffscreen(&rect))
			{
				// it's on screen, let's go with it
				curScreenshot = image;
				curScreenIdx = count;
				break;
			}
		}
	}

	viewSSButton->hidden = false;
	if (curScreenshot == NULL)
	{
		// if readmore isn't pressed, put the button there
		if (expandedReadMore)
		{
			viewSSButton->hidden = true;
		}
		else
		{
			auto readMoreText = i18n("contents.readmore");
			// no screenshot is on screen, so get our button out of there
			viewSSButton->position((banner->x + banner->width) / 2 - viewSSButton->width / 4, (details->y + details->height + 20));
			if (viewSSButton->getText() != readMoreText)
			{
				viewSSButton->updateText(readMoreText.c_str());
			}
			curScreenIdx = 0;
		}
	}
	else
	{
		// if we have a screenshot on screen, let's update the View button's position
		auto viewText = i18n("contents.view");
		if (viewSSButton->getText() != viewText)
		{
			viewSSButton->updateText(viewText.c_str());
		}
		viewSSButton->position(
			screenshotsContainer->x + curScreenshot->x + curScreenshot->width - viewSSButton->width - 5,
			screenshotsContainer->y + curScreenshot->y + curScreenshot->height - viewSSButton->height - 5);
		// std::cout << "viewSSButton position: " << viewSSButton.x << ", " << viewSSButton.y << std::endl;
	}

	ret = ret || ListElement::process(event);

	int maxScrollOffset = screenshotsContainer->y + screenshotsContainer->height - SCREEN_HEIGHT + 100;

	// make sure the max scroll offset is positive and snap <50 to 0
	if (maxScrollOffset < 50)
	{
		maxScrollOffset = 0;
	}

	// if we're not touch dragging, and we're out of bounds, reset scroll bounds
	if ((!event->isScrolling || event->isTouchUp()) && abs(this->y) > maxScrollOffset)
	{
		this->y = -maxScrollOffset;
	}

	return ret;
}

void AppDetailsContent::switchExtraInfo(std::shared_ptr<Package> package, int newState)
{

	// update button text
	auto hideText = i18n("contents.hide");
	auto showText = i18n("contents.show");
	showChangelog->updateText((std::string(newState == SHOW_CHANGELOG ? hideText : showText) + " " + i18n("contents.changelog")).c_str());

	// hide/show changelog text based on if neither is true
	changelog->hidden = newState == SHOW_NEITHER;
	extraContentState = newState;

	// update the "changelog" text depending on which action we're doing
	if (newState == SHOW_CHANGELOG)
	{
		changelog->setText(std::string(i18n("contents.changelog") + ":\n") + package->getChangelog().c_str());
		changelog->setFont(NORMAL);
		changelog->update();
	}

	// update the position of the screenshots based on text
	screenshotsContainer->position(
		screenshotsContainer->x,
		showFiles->y + showFiles->height + 30 + (changelog->hidden ? 0 : changelog->height));
}

std::vector<std::pair<std::string, std::string>> AppDetailsContent::getManifestFiles(std::shared_ptr<Package> package)
{
	std::vector<std::pair<std::string, std::string>> allEntries;
	int count = 0;
	// if it's an installed package, use the already installed manifest
	// (LOCAL -> UPDATE packages won't have a manifest)
	auto status = package->getStatus();
	if ((status == INSTALLED || status == UPDATE) && package->manifest.isValid())
	{
		allEntries.push_back({ "header1", i18n("contents.files.current") });
		for (auto& entry : package->manifest.getEntries())
		{
			allEntries.push_back({ "file1-" + std::to_string(count), entry.path });
			count += 1;
		}
		// allEntries["newline"] = "\n";
	}

	if (status != INSTALLED && !allEntries.empty())
	{
		// manifest is either non-local, or we need to display both, download it from the server
		std::string data("");
		std::string manifestUrl = package->getManifestUrl();
		if (!manifestUrl.empty() && downloadFileToMemory(manifestUrl.c_str(), &data))
		{
			allEntries.push_back({ "header2", i18n("contents.files.remote") });
			// split data into lines
			std::istringstream stream(data);
			std::string line;
			count = 0;
			while (std::getline(stream, line))
			{
				if (line.empty()) continue;
				allEntries.push_back({ "file2-" + std::to_string(count), line });
				count += 1;
			}
		}
		else
		{
			allEntries.push_back({ "error", "Failed to download manifest from server" });
		}
	}
	
	// always have at least one entry
	if (allEntries.empty())
	{
		allEntries.push_back({ "N/A", "N/A" });
	}

	return allEntries;
}