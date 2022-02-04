#if defined(SWITCH)
#include <switch.h>
#endif

#if defined(__WIIU__)
#include <romfs-wiiu.h>
#include <unistd.h>

#include <sysapp/launch.h>
#include <whb/log.h>
#include <whb/log_cafe.h>
#include <whb/log_udp.h>
#include <proc_ui/procui.h>

#include <unistd.h>
#include <sys/iosupport.h>
#endif

#include "../libs/get/src/Get.hpp"
#include "../libs/get/src/Utils.hpp"

#include "../gui/MainDisplay.hpp"

#include "../console/Menu.hpp"


#include "main.hpp"

void quit()
{
#ifdef __WIIU__
	SYSLaunchMenu();
#endif
	RootDisplay::mainDisplay->isRunning = false;
}

#ifdef __WIIU__
void elfMigrateLogic()
{
#define HBAS_PATH ROOT_PATH "wiiu/apps/appstore"
#define ELF_PATH HBAS_PATH "/hbas.elf"
#define RPX_PATH HBAS_PATH "/appstore.rpx"
  	mkdir(HBAS_PATH, 0700);
	chdir(HBAS_PATH);

	// "migrate" old elf users over to rpx (should've been done last version)
	struct stat sbuff;
	if (stat(ELF_PATH, &sbuff) == 0)
	{
		// hbas.elf exists... what should we do about it?
		if (stat(RPX_PATH, &sbuff) == 0)
		{
			// rpx is here, we can just delete hbas.elf
			// if we really have to, we can have wiiu-hbas.elf later for nostalgic people for the old version
			int re = std::remove(ELF_PATH);
			printf("Status removing folder... %d, %d: %s\n", re, errno, strerror(errno));
		}
		else
		{
			// no rpx, let's move our elf there
			int re = std::rename(ELF_PATH, RPX_PATH);
			printf("Status renaming folder... %d, %d: %s\n", re, errno, strerror(errno));
		}
	}
}
#endif

int main(int argc, char* argv[])
{
#if defined(__WIIU__)
	elfMigrateLogic();
#endif
	init_networking();

	bool cliMode = false;

#ifdef NOGUI
	cliMode = true;
#endif
	for (int x=0; x<argc; x++)
		if (std::string("--recovery") == argv[x])
			cliMode = true;

  	// initialize main title screen
	MainDisplay* display = new MainDisplay();
	display->canUseSelectToExit = true;

	auto events = display->events;
	events->quitaction = quit;

	for (int x=0; x<10; x++)
	{
		while (events->update()) {
			// check if L or R is pushed during startup
			cliMode |= (events->held(L_BUTTON) || events->held(R_BUTTON));
		}

		// small delay for recovery mode input
		CST_Delay(16);
	}

	if (cliMode) {
#ifndef SDL1
		// if NOGUI variable defined, use the console's main method
		// TODO: process InputEvents outside of MainDisplay, which might have more requirements
		int console_main(RootDisplay*, InputEvents*);
		console_main(display, events);
#endif
	}
	else
		// start primary app 
		display->mainLoop();
	
#if defined(SWITCH)
	socketExit();
#endif

#if defined(__WIIU__)
	// WHBLogUdpDeinit();
#endif

	return 0;
}
