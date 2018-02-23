#include "Menu.hpp"
#include <sstream>
#include <algorithm>

using namespace std;

Menu::Menu(Console* console, std::vector<Package*> packages)
{
	this->console = console;
	this->packages = packages;
	
	this->position = 2;
	this->offset = 0;
	
	this->screen = LIST_MENU;
	
	if (packages.size() > 0)
		this->repoUrl = packages[0]->repoUrl->c_str();
	else
		this->repoUrl = "No packages found on any repos!";
}

void Menu::display()
{
	if (this->screen == SPLASH)
	{
		
	}
	
	if (this->screen == LIST_MENU)
	{
		// draw the top bar
		console->fillRect(0, 0, 80, 1, 255, 255, 0);
		console->drawColorString(80/2 - 15, 0, "Homebrew App Store Preview", 0, 0, 0);

		for (int x=2; x<37; x+=3)
		{
			if (x >= packages.size()*3)
				break;
			
			Package* cur = packages[x/3];
			std::stringstream line;
			line << cur->title << " (" << cur->version << ")";
			console->drawString(15, x - offset, line.str().c_str());

			int r = (cur->status == UPDATE)? 0xFF : 0x00;
			int g = (cur->status == UPDATE)? 0xF7 : 0xFF;
			int b = (cur->status == INSTALLED)? 0xFF : 0x00;
			console->drawColorString(5, x - offset, cur->statusString(), r, g, b);
			
			console->drawColorString(17, x - offset + 1, cur->short_desc.c_str(), 0xcc, 0xcc, 0xcc);
		}
		
		std::stringstream footer;
		footer << "Page " << this->offset/12+1 << " of " << packages.size()/12;
		console->drawString(34, 40, footer.str().c_str());
		console->drawColorString(15, 42, "Use left/right and up/down to switch pages and apps", 0xcc, 0xcc, 0xcc);

		console->drawString(1, position - offset, "-->");
		
		console->fillRect(0, 44, 80, 1, 255, 255, 0);
		console->drawColorString(0, 44, "[A] Install   [B] Back", 0, 0, 0);
		console->drawColorString(80-(strlen(this->repoUrl)+2), 44, this->repoUrl, 0, 0, 0);

	}
}

void Menu::moveCursor(int diff)
{
	int old_position = position;
	this->position += diff;
}