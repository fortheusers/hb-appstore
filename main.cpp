#if defined(NOGUI)
	#include "console/Menu.hpp"
	#include "console/Input.hpp"
#else
	#include "gui/MainDisplay.hpp"
#endif

#include "libs/get/src/Utils.hpp"
#include "libs/get/src/Get.hpp"
//#include <switch.h>

int main(int argc, char *argv[])
{
//	consoleDebugInit(debugDevice_SVC);
//	stdout = stderr; // for yuzu

#if defined(NOGUI)
	// if NOGUI variable defined, use the console's main method
	int console_main(void);
	return console_main();
#else
	// initialize main title screen
	MainDisplay* display = new MainDisplay();
	
	bool running = true;
	while (running)
	{
		// get an event
		SDL_Event event;
		SDL_PollEvent(&event);
		
		// wait 16ms
		SDL_Delay(16);
		
		// process the inputs of the supplied event
		display->processInput(&event);
		
		// draw the display
		display->render(NULL);
		
		// quit on enter/start
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN)
			running = false;
	}
	
	return 0;
#endif
}
