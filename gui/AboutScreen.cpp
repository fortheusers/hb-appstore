#include <sstream>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "libget/src/Get.hpp"
#include "libget/src/Utils.hpp"

#include "chesto/src/Button.hpp"
#include "chesto/src/RootDisplay.hpp"
#include "chesto/src/NetImageElement.hpp"


#include "AboutScreen.hpp"
#include "Feedback.hpp"

#define AVATAR_URL "https://avatars.githubusercontent.com/u/"

AboutScreen::AboutScreen(Get* get)
{
	this->get = get;

	SDL_Color red = { 0xFF, 0x00, 0x00, 0xff };
	SDL_Color gray = { 0x50, 0x50, 0x50, 0xff };
	SDL_Color black = { 0x00, 0x00, 0x00, 0xff };
	SDL_Color white = { 0xFF, 0xFF, 0xFF, 0xff };

	// TODO: show current app status somewhere

	// download/update/remove button (2)

	Button* cancel = new Button("Go Back", B_BUTTON, false, 29);
	cancel->position(30, 30);
	cancel->action = std::bind(&AboutScreen::back, this);
	this->elements.push_back(cancel);

	// Button* cleanup = new Button("Cleanup Empty Folders", Y_BUTTON, true, 21);
	// cleanup->position(30, 500);
	// cleanup->action = std::bind(&AboutScreen::removeEmptyFolders, this);
	// this->elements.push_back(cleanup);

	// Button* cache = new Button("Delete Image Cache", X_BUTTON, true, 21, cleanup->width);
	// cache->position(30, cleanup->y + cleanup->height + 25);
	// cache->action = std::bind(&AboutScreen::wipeCache, this);
	// this->elements.push_back(cache);

	int MARGIN = 550;

	Button* feedback = new Button("Leave Feedback", A_BUTTON, false, 17);
	feedback->position(MARGIN + 500, 30);
	feedback->action = std::bind(&AboutScreen::launchFeedback, this);
	this->elements.push_back(feedback);

	TextElement* title = new TextElement("Homebrew App Store", 35, &black);
	title->position(MARGIN, 40);
	this->elements.push_back(title);

	TextElement* subtitle = new TextElement("by fortheusers.org", 25, &black);
	subtitle->position(MARGIN, 80);
	this->elements.push_back(subtitle);

  NetImageElement* ftuLogo = new NetImageElement(AVATAR_URL "40721862");
  ftuLogo->position(375, 15);
  ftuLogo->resize(140, 140);
  this->elements.push_back(ftuLogo);

	const char* blurb = "Licensed under the GPLv3 license. This app is free and open source because the users (like you!) deserve it.\n\n\Let's support homebrew and the right to control what software we run on our own devices!";

	TextElement* creds = new TextElement(blurb, 20, &black, false, 1240);
	creds->position(100, 170);
	this->elements.push_back(creds);

	// argument order:
	// username, githubIdOrImageUrl, twitter, github, gitlab, patreon, url, discord
	// only first two social points will be used

	credHead("Repo Maintainance and Development", "These are the primary people responsible for actively maintaining and developing the Homebrew App Store. If there's a problem, these are the ones to get in touch with!");
	credit("pwsincd", "20027105", NULL, "pwsincd", NULL, NULL, NULL, "pwsincd#9044");
	credit("VGMoose", "2467473", "vgmoose", "vgmoose");
	credit("rw-r-r_0644", "18355947", "rw_r_r_0644", "rw-r-r-0644");
	credit("crc32", "7893269", "crc32_", "crc-32");
	credit("CompuCat", "12215288", NULL, NULL, "compucat", NULL, "compucat.me");
	credit("Quarky", "8533313", NULL, NULL, "quarktheawesome", NULL, "heyquark.com");
	credit("Whovian9369", "5240754", NULL, NULL, "whovian9369");

	credHead("Library Development and Support", "Without the contributions to open-source libraries and projects by these people, much of the functionality within this program wouldn't be possible.");
	credit("Maschell", "8582508", "maschelldev", "maschell");
	credit("brienj", "17801294", "xhp_creations", "xhp-creations");
	credit("Dimok", "15055714", NULL, "dimok789");
	credit("FIX94", "12349638", NULL, "FIX94", NULL, NULL, NULL, "FIX94#3446");
	credit("Zarklord", "1622280", "zarklore", "zarklord");
	credit("CreeperMario", "15356475", "CreeperMario258", "CreeperMario");
	credit("Ep8Script", "27195853", "ep8script", "ep8script");

	credHead("Interface Development and Design", "In one way or another, everyone in this category provided information regarding core functionality, quality-of-life changes, or the design of the user interface.");
	credit("exelix", "13405476", "exelix11", "exelix11");
	credit("Xortroll", "33005497", NULL, "xortroll", NULL, "xortroll");
	credit("Ave", "https://gitlab.com/uploads/-/system/user/avatar/584369/avatar.png", NULL, NULL, "a", NULL, "ave.zone");
	credit("LyfeOnEdge", "26140376", NULL, "lyfeonedge", NULL, NULL, NULL, "Lyfe#1555");
	credit("Román", "57878194", NULL, NULL, NULL, NULL, NULL, "Román#6630");
	credit("Jaames", "9112876", "rakujira", "jaames");
	credit("Jacob", "12831497", NULL, "jacquesCedric");
	credit("iTotalJustice", "47043333", NULL, "iTotalJustice");

	credHead("Toolchain and Environment", "The organizations and people in this category enable Homebrew in general by creating and maintaining a cohesive environment for the community.");
	credit("devkitPro", "7538897", NULL, "devkitPro", NULL, "devkitPro");
	credit("Wintermute", "101194", NULL, "wintermute", NULL, NULL, "devkitPro.org");
	credit("Fincs", "581494", "fincsdev", "fincs");
	credit("yellows8", "585494", "yellows8");
	credit("ReSwitched", "26338222", NULL, "reswitched", NULL, NULL, "reswitched.team");
	credit("exjam", "1302758", NULL, "exjam");

	credHead("Homebrew Community Special Thanks", "Awesome people within the community whose work, words, or actions in some way inspired this program to exist in the manner it does.");
	credit("misson20000", "616626", NULL, "misson20000", NULL, NULL, NULL, "misson20000#0752");
	credit("roblabla", "1069318", NULL, "roblabla", NULL, NULL, NULL, "roblabla#8145");
	credit("tomGER", "25822956", "tumGER", "tumGER");
	credit("m4xw", "13141469", "m4xwdev", "m4xw");
	credit("Nikki", "3280345", "NWPlayer123", "NWPlayer123");
	credit("shchmue", "7903403", NULL, "shchmue");
	credit("CTCaer", "3665130", "CTCaer", "CTCaer");
	credit("SciresM", "8676005", "SciresM", "SciresM");
	credit("Shinyquagsire", "1224096", "shinyquagsire", "shinyquagsire23");
	credit("Marionumber1", "775431", "MrMarionumber1");
}

void AboutScreen::credHead(const char* header, const char* blurb)
{
	SDL_Color black = { 0x00, 0x00, 0x00, 0xff };
	SDL_Color gray = { 0x50, 0x50, 0x50, 0xff };

	creditCount += (4 - creditCount%4) % 4;
	TextElement* text = new TextElement(header, 30, &black);
	text->position(40, 250 + 60 + creditCount / 4 * 160);
	this->elements.push_back(text);

	TextElement* desc = new TextElement(blurb, 23, &gray, false, 1200);
	desc->position(40, 250 + 105 + creditCount / 4 * 160);
	this->elements.push_back(desc);

	creditCount += 4;
}

void AboutScreen::credit(const char* username,
												const char* githubIdOrUrl,
												const char* twitter,
												const char* github,
												const char* gitlab,
												const char* patreon,
												const char* url,
												const char* discord)
{
	int X = 40;
	int Y = 310;

	SDL_Color gray = { 0x50, 0x50, 0x50, 0xff };
	SDL_Color black = { 0x00, 0x00, 0x00, 0xff };

	int myX = creditCount % 4 * 300 + X;
	int myY = creditCount / 4 * 160 + Y;

	NetImageElement* userLogo = new NetImageElement((std::string(AVATAR_URL) + githubIdOrUrl).c_str(), [githubIdOrUrl]{
			return new NetImageElement(githubIdOrUrl);
		});
  userLogo->position(myX, myY);
  userLogo->resize(100, 100);
  this->elements.push_back(userLogo);

	TextElement* name = new TextElement(username, 27, &black);
	name->position(myX + 110, myY);
	this->elements.push_back(name);

	int socialCount = 0;

	const char * handles[6] = { twitter, github, gitlab, patreon, url, discord };
	const char * icons[6] = { "twitter", "github", "gitlab", "patreon", "url", "discord" };

	for (int x=0; x<6; x++) {
		if (handles[x] == NULL) continue;

		ImageElement* icon = new ImageElement(((std::string(ROMFS "res/") + icons[x]) + ".png").c_str());
		icon->resize(20, 20);
		icon->position(myX + 110, myY + 45 + socialCount*25);
		this->elements.push_back(icon);

		TextElement* link = new TextElement(handles[x], 14, &gray);
		link->position(myX + 140, myY + 45 + socialCount*25);
		this->elements.push_back(link);

		socialCount++;

		if (socialCount >= 2) break;
	}

	creditCount++;
}

void AboutScreen::render(Element* parent)
{
	if (this->parent == NULL)
		this->parent = parent;

	// draw a white background, 1280 wide
	SDL_Rect dimens = { 0, 0, 1280, 720 };

	SDL_SetRenderDrawColor(parent->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(parent->renderer, &dimens);
	this->renderer = parent->renderer;

	super::render(this);
}

void AboutScreen::back()
{
	RootDisplay::subscreen = NULL; // TODO: clean up memory?
}

// void AboutScreen::removeEmptyFolders()
// {
// 	remove_empty_dirs(ROOT_PATH, 0);
// }

// void AboutScreen::wipeCache()
// {
// 	// clear out versions
// 	std::remove(".get/tmp/cache/versions.json");
// }

void AboutScreen::launchFeedback()
{
	// find the package corresponding to us
	for (auto& package : this->get->packages)
	{
		if (package->pkg_name == "appstore")
		{
			RootDisplay::subscreen = new Feedback(package);
			break;
		}
	}
}
