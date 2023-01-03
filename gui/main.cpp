#if defined(SWITCH)
#include <switch.h>
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

#include "../gui/MainDisplay.hpp"

#include "../console/Menu.hpp"

#include "main.hpp"

void quit()
{
#ifdef __WIIU__
	// will exit via procui loop in RootDisplay
	SYSLaunchMenu();
#else
	RootDisplay::mainDisplay->isRunning = false;
#endif
}

#ifdef __WIIU__
void wiiuSetPwd()
{
#define HBAS_PATH ROOT_PATH "wiiu/apps/appstore"
#define ELF_PATH HBAS_PATH "/hbas.elf"
#define RPX_PATH HBAS_PATH "/appstore.rpx"
	// create and cd into the appstore directory
	mkdir(HBAS_PATH, 0700);
	chdir(HBAS_PATH);
}
#endif

int main(int argc, char* argv[])
{
#if defined(__WIIU__)
	wiiuSetPwd();
#endif
	init_networking();

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
	events->quitaction = quit;

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
#ifndef SDL1
		// if NOGUI variable defined, use the console's main method
		// TODO: process InputEvents outside of MainDisplay, which might have more requirements
		int console_main(RootDisplay*, InputEvents*);
		console_main(display, events);
#endif
	}
	else
	{
		display->setupMusic();

		// start primary app
		display->mainLoop();
	}

	deinit_networking();

	return 0;
}
