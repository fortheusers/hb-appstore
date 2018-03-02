#include "Menu.hpp"
#include <sstream>
#include <algorithm>

using namespace std;

Menu::Menu(Console* console, std::vector<Package*> packages)
{
	this->console = console;
	this->packages = packages;
	
	this->position = 0;
	this->offset = 0;
	
	this->screen = SPLASH;
	
	if (packages.size() > 0)
		this->repoUrl = packages[0]->repoUrl->c_str();
	else
		this->repoUrl = "No packages found on any repos!";
}

void Menu::display()
{
	if (this->screen == SPLASH)
	{
		console->drawString(9, 21, "Homebrew App Store (Preview!)");
		console->drawColorString(9, 26, "thanks to:", 0xcc, 0xcc, 0xcc);
		console->drawColorString(15, 27, "vgmoose, pwsincd, rw-r-r_0644, zarklord, kgsws", 0xcc, 0xcc, 0xcc);
		console->drawColorString(9, 32, "Press [A] to continue", 0xff, 0xff, 0x00);
//		console->drawColorString(9, 31, "GPLv3 License", 0xcc, 0xcc, 0xcc);
//		console->drawColorString(15, 27, "    vgmoose - package manager and interface", 0xcc, 0xcc, 0xcc);
//		console->drawColorString(15, 28, "    pwsincd - domain, hosting, and more", 0xcc, 0xcc, 0xcc);
//		console->drawColorString(15, 29, "rw-r-r_0644 - manifest parsing logic", 0xcc, 0xcc, 0xcc);
//		console->drawColorString(15, 30, "   zarklord - zip folder extraction library", 0xcc, 0xcc, 0xcc);
//		console->drawColorString(15, 31, "      kgsws - HTTP downloading code", 0xcc, 0xcc, 0xcc);
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
			
			std::stringstream line2;
			line2  << cur->short_desc << " [" << cur->author << "]";
			
			console->drawColorString(16, x - offset + 1, line2.str().c_str(), 0xcc, 0xcc, 0xcc);
		}
		
		std::stringstream footer;
		footer << "Page " << this->offset/12+1 << " of " << packages.size()/12+1;
		console->drawString(34, 40, footer.str().c_str());
		console->drawColorString(15, 42, "Use left/right and up/down to switch pages and apps", 0xcc, 0xcc, 0xcc);

		console->drawString(1, 2 + position*3 - offset, "-->");
		
		console->fillRect(0, 44, 80, 1, 255, 255, 0);
		console->drawColorString(0, 44, "[A] Install   [B] Back", 0, 0, 0);
		console->drawColorString(80-(strlen(this->repoUrl)+2), 44, this->repoUrl, 0, 0, 0);

	}
	
	console->update();
}

void Menu::moveCursor(int diff)
{
	int old_position = position;
	this->position += diff;
	
	if (position < 0)
	{
		// try to go back a page
		position = 11;
	}
	
	if (position > 11)
	{
		// try to go forwards a page
		position = 0;
	}
}

void Menu::advanceScreen()
{
	if (this->screen == SPLASH)
		this->screen = LIST_MENU;
}