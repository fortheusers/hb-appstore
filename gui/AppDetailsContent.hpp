#ifndef APPDETAILSCONTENT_H_
#define APPDETAILSCONTENT_H_

#include "../libs/get/src/Get.hpp"
#include "../libs/get/src/Package.hpp"

#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/ListElement.hpp"
#include "../libs/chesto/src/ProgressBar.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "../libs/chesto/src/NetImageElement.hpp"
#include "../libs/chesto/src/Container.hpp"
#include "../libs/chesto/src/DropDown.hpp"

#define SHOW_NEITHER 0
#define SHOW_CHANGELOG 1
#define SHOW_LIST_OF_FILES 2

class AppDetailsContent : public ListElement
{
public:
	AppDetailsContent(Package *package, bool lowMemoryMode, DropDownControllerElement* controller);
	bool process(InputEvents* event);
	void render(Element* parent);
	void switchExtraInfo(Package* package, int newState);
	void slideUIDown(int heightOffset);
	std::vector<std::pair<std::string, std::string>> getManifestFiles(Package* package);

	Button reportIssue;
	Button moreByAuthor;

	CST_Color gray = { 0x50, 0x50, 0x50, 0xff };
	CST_Color black = { 0x00, 0x00, 0x00, 0xff };
	
	bool showingScreenshot = false;
	bool expandedReadMore = false;

private:
	// banner/text constants
	const int MARGIN = 60;
	const int BANNER_X = MARGIN + 5;
	const int BANNER_Y = 140;

	TextElement title;
	TextElement title2;
	TextElement details;
	TextElement changelog;
	NetImageElement banner;

	DropDown showFiles;
	Button showChangelog;
	Button viewSSButton;
	Container screenshotsContainer;

	int extraContentState = SHOW_NEITHER;
	int curScreenIdx = 0;
};
#endif