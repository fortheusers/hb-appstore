#include "Menu.hpp"
#include "Input.hpp"
#include "libs/get/src/Utils.hpp"
#include "libs/get/src/Get.hpp"

int main()
{
	// initialize text console
	Console* console = new Console();
	
	init_networking();
	
	Get* get = new Get("./.get/", "http://192.168.1.104/appstore");
	Input* input = new Input();
	
	Menu* menu = new Menu(console, get->packages);
	
	bool running = true;
		
	while(running)
	{
		SDL_Delay(16);
		console->background(42, 37, 39);

		input->updateButtons();
		
		menu->display();
		
		if (input->held(BUTTON_A))
			menu->advanceScreen();
		
		if (input->held(BUTTON_PLUS))
			running = false;
		
		menu->moveCursor(-1*(input->held(BUTTON_UP)) + (input->held(BUTTON_DOWN)));
	}
	
	console->close();
	input->close();

	return 0;
}
