#ifndef APPDETAILS_H_
#define APPDETAILS_H_
#include <memory>
#include "../libs/chesto/src/DropDown.hpp"
#include "../libs/get/src/Utils.hpp"

#include "AppCard.hpp"
#include "AppDetailsContent.hpp"

using namespace Chesto;

class AppList;

class AppDetails : public Screen
{
public:
	AppDetails(Package& package, AppList* appList, AppCard* appCard = NULL);
	~AppDetails();

	std::string getPackageDetails(Package* package);
	std::string getAction(Package* package);
	bool process(InputEvents* event) override;
	void render(Element* parent) override;
	void rebuildUI() override;
	bool launchFile(char* path, char* context);
	bool themeInstall(char* installerPath);
	void getSupported();

	CST_Color red = { 0xFF, 0x00, 0x00, 0xff };
	CST_Color gray = { 0x50, 0x50, 0x50, 0xff };
	CST_Color black = { 0x00, 0x00, 0x00, 0xff };
	CST_Color white = { 0xFF, 0xFF, 0xFF, 0xff };

	bool operating = false;
	std::shared_ptr<Package> package;
	Get* get = NULL;
	AppList* appList = NULL;
	AppCard* appCard = NULL;
	ProgressBar* pbar = NULL;
	int highlighted = -1;

	bool canLaunch = false;

	// the callback method to update the currently displayed pop up (and variables it needs)
	static int updateCurrentlyDisplayedPopup(void* clientp, double progress);
	static int updatePopupStatus(int status, int num = 1, int num_total = 1);
	static int lastFrameTime;

	void proceed();
	void back();
	void launch();

	void moreByAuthor();
	void leaveFeedback();

	void preInstallHook();
	void postInstallHook();
	
	void showResumePrompt(int percentComplete);
	void startInstallOrRemove(bool resume);

	// on some platform + package combinations, we need to quit after installing
	bool quitAfterInstall = false;

private:
	Button* start = nullptr;
	TextElement* errorText = nullptr;
	TextElement* details = nullptr;
	AppDetailsContent* content = nullptr;

	Button* download = nullptr;
	Button* cancel = nullptr;
};

#endif
