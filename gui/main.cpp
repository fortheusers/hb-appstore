#if defined(SWITCH)
#include <switch.h>
#endif

#if defined(WII)
#include <stdlib.h>
#include <unistd.h>
#include <fat.h>
// Handles basic HW Init,
// Including Wiimote as Mouse
// And starting the Fat FS
#include "SDL2/SDL_main.h"
#include "../libs/chesto/src/DrawUtils.hpp"
#include <ogc/system.h>
#endif

#if defined(__WIIU__)
#include <unistd.h>

#include <proc_ui/procui.h>
#include <sysapp/launch.h>
#include <whb/log.h>
#include <whb/log_cafe.h>
#include <whb/log_udp.h>

#include <sys/iosupport.h>
#include <unistd.h>
#endif

#include "../libs/get/src/Get.hpp"
#include "../libs/get/src/Utils.hpp"

#include "ThemeManager.hpp"
#include "../gui/MainDisplay.hpp"

#include "../console/Menu.hpp"

#include "main.hpp"

#if defined(__WIIU__)
void setPlatformPwd()
{
#define HBAS_PATH ROOT_PATH "wiiu/apps/appstore"

	// create and cd into the appstore directory
	mkpath(HBAS_PATH);
	chdir(HBAS_PATH);
}
#endif

#if defined(WII)
void setPlatformPwd()
{
#define HBAS_PATH ROOT_PATH "apps/appstore"

	// create and cd into the appstore directory
	mkpath(HBAS_PATH);
	chdir(HBAS_PATH);
}
#endif

int main(int argc, char* argv[])
{
	#ifdef WII
	SYS_STDIO_Report(true);
	#endif
#if defined(__WIIU__) || defined(WII)
	setPlatformPwd();
#endif
	init_networking();
	setUserAgent("HBAS/" APP_VERSION " (" PLATFORM "; Chesto)");
	HBAS::ThemeManager::themeManagerInit();

	bool cliMode = false;

#ifdef NOGUI
	cliMode = true;
#endif
	for (int x = 0; x < argc; x++)
		if (std::string("--recovery") == argv[x])
			cliMode = true;

	// initialize main title screen
	MainDisplay* display = new MainDisplay();
	display->canUseSelectToExit = true;

	auto events = display->events;

	for (int x = 0; x < 10; x++)
	{
		while (events->update())
		{
			// check if L or R is pushed during startup
			cliMode |= (events->held(L_BUTTON) || events->held(R_BUTTON));
		}

		// small delay for recovery mode input
		CST_Delay(16);
	}

	if (cliMode)
	{
		// if NOGUI variable defined, use the console's main method
		// TODO: process InputEvents outside of MainDisplay, which might have more requirements
		int console_main(RootDisplay*, InputEvents*);
		console_main(display, events);
	}
	else
	{
		display->setupMusic();

		#if defined(WII)
		// Wii uses a Hand Cursor by default, so force Arrow
		CST_SetCursor(CST_CURSOR_ARROW);
		#endif

		// start primary app
		display->mainLoop();
	}

	deinit_networking();

	return 0;
}
