#if defined(NOGUI)
	#include "console/Menu.hpp"
	#include "console/Input.hpp"
#else
	#include "gui/MainDisplay.hpp"
#endif

#include "libs/get/src/Utils.hpp"
#include "libs/get/src/Get.hpp"

#if defined(SWITCH)
	#include <switch.h>
#endif

int main(int argc, char *argv[])
{
//	consoleDebugInit(debugDevice_SVC);
//	stdout = stderr; // for yuzu

#if defined(NOGUI)
	// if NOGUI variable defined, use the console's main method
	int console_main(void);
	return console_main();
#else
	init_networking();

	// create main get object
	Get* get = new Get("./.get/", "http://switchbru.com/appstore");

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
		while(events->update())
        {
            // process the inputs of the supplied event
            viewChanged |= display->process(events);
            atLeastOneNewEvent = true;
        }
        
        // one more event update if nothing changed or there were no previous events seen
        // needed to non-input related processing that might update the screen to take place
        if (!atLeastOneNewEvent && !viewChanged)
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

		// quit on enter/start
		if (events->held(START_BUTTON))
			running = false;
	}

	IMG_Quit();
	TTF_Quit();

	SDL_Delay(10);
	SDL_DestroyWindow(display->window);

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();

	#if defined(SWITCH)
		socketExit();
	#endif

	return 0;
#endif
}
