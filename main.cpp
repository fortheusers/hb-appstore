#include "Menu.hpp"
#include "libs/get/src/Get.hpp"

int main()
{
	// initialize text console
	Console* console = new Console();

	Get* get = new Get("./.get/", "http://switchbru.com/appstore/");
	Menu* menu = new Menu(console, get->packages);
	
	console->background(42, 37, 39);
	
	while(true)
	{
		menu->display();
		SDL_Delay(16);
	}
	
	console->close();

	return 0;
}
