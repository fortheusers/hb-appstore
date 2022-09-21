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

class AppDetailsContent : public ListElement
{
public:
	AppDetailsContent(Package *package, bool lowMemoryMode);
	bool process(InputEvents* event);
	void render(Element* parent);

	Button reportIssue;
	Button moreByAuthor;

	CST_Color gray = { 0x50, 0x50, 0x50, 0xff };
	CST_Color black = { 0x00, 0x00, 0x00, 0xff };
	bool showingScreenshot = false;

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

	Button showFiles;
	Button showScreenshots;
	Button viewSSButton;
	Container screenshotsContainer;

	bool showingFilesList = false;
	bool showingScreenshots = false;
	int curScreenIdx = 0;
};
#endif