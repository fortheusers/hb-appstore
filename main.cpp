#if defined(SWITCH)
#include <switch.h>
#endif

#if defined(__WIIU__)
#include <unistd.h>
#endif

#include "libget/src/Get.hpp"
#include "libget/src/Utils.hpp"

#if defined(NOGUI)
#include "console/Input.hpp"
#include "console/Menu.hpp"
#else
#include "gui/MainDisplay.hpp"
#endif

#if defined(__WIIU__)
#define DEFAULT_REPO "http://wiiubru.com/appstore"
#else
#define DEFAULT_REPO "http://switchbru.com/appstore"
#endif

int main(int argc, char* argv[])
{
//	consoleDebugInit(debugDevice_SVC);
//	stdout = stderr; // for yuzu

#if defined(__WIIU__)
	const char* wiiu_home = "fs:/vol/external01/wiiu/apps/appstore";
	mkdir(wiiu_home, 0700);
	chdir(wiiu_home);
#endif
	init_networking();

	// create main get object
	Get* get = new Get("./.get/", DEFAULT_REPO);

#if defined(NOGUI)
	// if NOGUI variable defined, use the console's main method
	int console_main(Get*);
	return console_main(get);
#else

	// initialize main title screen
	MainDisplay* display = new MainDisplay(get);

	// the main inuput handler
	InputEvents* events = new InputEvents();

	bool running = true;
	while (running)
	{
		bool atLeastOneNewEvent = false;
		bool viewChanged = false;

		int frameStart = SDL_GetTicks();

		// get any new input events
		while (events->update())
		{
			// process the inputs of the supplied event
			viewChanged |= display->process(events);
			atLeastOneNewEvent = true;

			// quit on select/minus
			if (events->held(SELECT_BUTTON))
				running = false;
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
			int delayTime = (SDL_GetTicks() - frameStart);
			if (delayTime < 0)
				delayTime = 0;
			if (delayTime < 16)
				SDL_Delay(16 - delayTime);
		}
	}

	quit();

	return 0;
#endif
}
