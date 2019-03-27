#ifndef APPDETAILS_H_
#define APPDETAILS_H_

#include "libget/src/Get.hpp"
#include "libget/src/Package.hpp"

#include "ImageElement.hpp"
#include "ListElement.hpp"
#include "ProgressBar.hpp"
#include "TextElement.hpp"

class AppList;

class AppDetails : public Element
{
public:
	AppDetails(Package* package, AppList* appList);
	bool process(InputEvents* event);
	void render(Element* parent);
	bool launchFile(char* path, char* context);
	bool themeInstall(char* installerPath);
	void getSupported();

	SDL_Color red = { 0xFF, 0x00, 0x00, 0xff };
	SDL_Color gray = { 0x50, 0x50, 0x50, 0xff };
	SDL_Color black = { 0x00, 0x00, 0x00, 0xff };
	SDL_Color white = { 0xFF, 0xFF, 0xFF, 0xff };

	bool operating = false;
	Package* package;
	Get* get;
	AppList* appList;
	ProgressBar* pbar = NULL;
	int highlighted = -1;

	bool canLaunch = false;

	// the callback method to update the currently displayed pop up (and variables it needs)
	static int updateCurrentlyDisplayedPopup(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow);
	static int lastFrameTime;

	void proceed();
	void back();
	void launch();

	void moreByAuthor();
	void leaveFeedback();

	void preInstallHook();
	void postInstallHook();
};

class AppDetailsContent : public ListElement
{
	bool process(InputEvents* event);
	void render(Element* parent);

public:
	bool useIconBanner = false;
	ImageElement* banner = NULL;

	// banner/text constants
	int MARGIN = 60;
	int BANNER_X = MARGIN + 5;
	int BANNER_Y = 140;
};

Uint32 getpixel(SDL_Surface* surface, int x, int y);

#endif
