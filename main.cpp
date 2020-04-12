#if defined(SWITCH)
#include <switch.h>
#endif

#if defined(__WIIU__)
#include <romfs-wiiu.h>
#include <unistd.h>


#include <unistd.h>
#include <sys/iosupport.h>
#endif

#include "libs/get/src/Get.hpp"
#include "libs/get/src/Utils.hpp"
#include "libs/chesto/src/DownloadQueue.hpp"

#include "gui/MainDisplay.hpp"

#include "console/Menu.hpp"


#include "main.hpp"

static bool running = true;

void quit()
{
	running = false;
}

int main(int argc, char* argv[])
{
	// consoleDebugInit(debugDevice_SVC);
	// stdout = stderr; // for yuzu

#if defined(__WIIU__)
	// WHBLogUdpInit();
	// WHBLogCafeInit();

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
#endif

	init_networking();
  // nxlinkStdio();

  bool cliMode = false;
#ifdef NOGUI
  cliMode = true;
#endif
  for (int x=0; x<argc; x++)
    if (std::string("--recovery") == argv[x])
      cliMode = true;

  // initialize main title screen
	MainDisplay* display = new MainDisplay();

	// the main input handler
	InputEvents* events = new InputEvents();
  for (int x=0; x<10; x++) {
    while (events->update()) {
      cliMode |= (events->held(L_BUTTON) || events->held(R_BUTTON));
    }
    CST_Delay(16);
  }

  if (cliMode)
  {
#ifndef SDL1
    // if NOGUI variable defined, use the console's main method
    int console_main(RootDisplay*, InputEvents*);
    console_main(display, events);
    running = false;
#endif
  }
	else
		// start music (only if MUSIC defined)
		display->initAndStartMusic();

	DownloadQueue::init();

	events->quitaction = quit;

	while (running)
	{
		bool atLeastOneNewEvent = false;
		bool viewChanged = false;

		int frameStart = CST_GetTicks();

		// update download queue
		DownloadQueue::downloadQueue->process();

		// get any new input events
		while (events->update())
		{
			// process the inputs of the supplied event
			viewChanged |= display->process(events);
			atLeastOneNewEvent = true;

      // if we see a minus, exit immediately!
      if (events->pressed(SELECT_BUTTON))
        quit();
		}

		// one more event update if nothing changed or there were no previous events seen
		// needed to non-input related processing that might update the screen to take place
		if ((!atLeastOneNewEvent && !viewChanged) || display->showingSplash)
		{
			events->update();
			viewChanged |= display->process(events);
		}

		// draw the display if we processed an event or the view
		if (viewChanged)
			display->render(NULL);
		else
		{
			// delay for the remainder of the frame to keep up to 60fps
			// (we only do this if we didn't draw to not waste energy
			// if we did draw, then proceed immediately without waiting for smoother progress bars / scrolling)
			int delayTime = (CST_GetTicks() - frameStart);
			if (delayTime < 0)
				delayTime = 0;
			if (delayTime < 16)
				CST_Delay(16 - delayTime);
		}
	}

	delete events;
	delete display;

	DownloadQueue::quit();

#if defined(SWITCH)
	socketExit();
#endif

#if defined(__WIIU__)
	// WHBLogUdpDeinit();
#endif

	return 0;
}
