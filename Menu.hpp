#include "libs/get/src/Get.hpp"
#include "Console.hpp"

#define SPLASH 0
#define CATEGORIES 1
#define LIST_MENU 2
#define INSTALL_SCREEN 3

class Menu
{
public:
	Menu(Console* console, std::vector<Package*> packages);
	void display();			// display the menu as it's set up
	void moveCursor(int diff);	// move the cursor position up (-1) or down (1)
	void advanceScreen();		// change the active screen to the next one
	
private:
	int offset;		// the offset of "scroll" along the current menu page
	int position;	// the position of the cursor along the current menu
	std::vector<Package*> packages;		// list of packages to draw
	
	int screen;
	const char* repoUrl;
	
	Console* console;	// link to the console
};