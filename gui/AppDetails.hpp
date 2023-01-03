#ifndef APPDETAILS_H_
#define APPDETAILS_H_

#include "AppDetailsContent.hpp"

class AppList;

class AppDetails : public Element
{
public:
	AppDetails(Package* package, AppList* appList);
	~AppDetails();

	std::string getPackageDetails(Package* package);
	const char* getAction(Package* package);
	bool process(InputEvents* event);
	void render(Element* parent);
	bool launchFile(char* path, char* context);
	bool themeInstall(char* installerPath);
	void getSupported();

	CST_Color red = { 0xFF, 0x00, 0x00, 0xff };
	CST_Color gray = { 0x50, 0x50, 0x50, 0xff };
	CST_Color black = { 0x00, 0x00, 0x00, 0xff };
	CST_Color white = { 0xFF, 0xFF, 0xFF, 0xff };

	bool operating = false;
	Package* package;
	Get* get;
	AppList* appList;
	ProgressBar* pbar = NULL;
	int highlighted = -1;

	bool canLaunch = false;

	// the callback method to update the currently displayed pop up (and variables it needs)
	static int updateCurrentlyDisplayedPopup(void* clientp, double dltotal, double dlnow, double ultotal, double ulnow);
	static int updatePopupStatus(int status, int num = 1, int num_total = 1);
	static int lastFrameTime;

	void proceed();
	void back();
	void launch();

	void moreByAuthor();
	void leaveFeedback();

	void preInstallHook();
	void postInstallHook();

	ProgressBar downloadProgress;

	// on some platform + package combinations, we need to quit after installing
	bool quitAfterInstall = false;

private:
#if defined(SWITCH)
	Button* start = nullptr;
	TextElement* errorText = nullptr;
#endif
	TextElement details;
	AppDetailsContent content;
	TextElement downloadStatus;

	Button download;
	Button cancel;
};

#endif
