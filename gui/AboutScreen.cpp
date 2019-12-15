#include <sstream>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "../libs/get/src/Get.hpp"
#include "../libs/get/src/Utils.hpp"

#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/RootDisplay.hpp"
#include "../libs/chesto/src/NetImageElement.hpp"


#include "AboutScreen.hpp"
#include "Feedback.hpp"

#define AVATAR_URL "https://avatars.githubusercontent.com/u/"


SDL_Color AboutScreen::black = { 0x00, 0x00, 0x00, 0xff };
SDL_Color AboutScreen::gray = { 0x50, 0x50, 0x50, 0xff };

AboutScreen::AboutScreen(Get* get)
	: get(get)
	, cancel("Go Back", B_BUTTON, false, 29)
	, feedback("Leave Feedback", A_BUTTON, false, 17)
	, title("Homebrew App Store", 35, &black)
	, subtitle("by fortheusers.org", 25, &black)
	, ftuLogo(AVATAR_URL "40721862", []{
	        return new ImageElement(RAMFS "res/4TU.png");
        })
	, creds("Licensed under the GPLv3 license. This app is free and open source because the users (like you!) deserve it.\n\nLet's support homebrew and the right to control what software we run on our own devices!",
			20, &black, false, 1240)
{

	// TODO: show current app status somewhere

	// download/update/remove button (2)

	cancel.position(30, 30);
	cancel.action = std::bind(&AboutScreen::back, this);
	super::append(&cancel);

	// Button* cleanup = new Button("Cleanup Empty Folders", Y_BUTTON, true, 21);
	// cleanup->position(30, 500);
	// cleanup->action = std::bind(&AboutScreen::removeEmptyFolders, this);
	// super::append(cleanup);

	// Button* cache = new Button("Delete Image Cache", X_BUTTON, true, 21, cleanup->width);
	// cache->position(30, cleanup->y + cleanup->height + 25);
	// cache->action = std::bind(&AboutScreen::wipeCache, this);
	// super::append(cache);

	int MARGIN = 550;

	feedback.position(MARGIN + 500, 30);
	feedback.action = std::bind(&AboutScreen::launchFeedback, this);
	super::append(&feedback);

	title.position(MARGIN, 40);
	super::append(&title);

	subtitle.position(MARGIN, 80);
	super::append(&subtitle);

	ftuLogo.position(375, 15);
	ftuLogo.resize(140, 140);
	super::append(&ftuLogo);

	creds.position(100, 170);
	super::append(&creds);

	// argument order:
	// username, githubId, twitter, github, gitlab, patreon, url, discord, directAvatarURL
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
	credit("Ave", "584369", NULL, NULL, "a", NULL, "ave.zone", NULL, "https://gitlab.com/uploads/-/system/user/avatar/584369/avatar.png");
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
  credit("brett19", "1621627", NULL, "brett19");

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

AboutScreen::~AboutScreen()
{
	super::removeAll();
	for (auto& i : creditHeads)
	{
		delete i.text;
		delete i.desc;
	}
	for (auto& i : credits)
	{
		delete i.userLogo;
		delete i.name;
		delete i.social[0].icon;
		delete i.social[0].link;
		delete i.social[1].icon;
		delete i.social[1].link;
	}
}

void AboutScreen::credHead(const char* header, const char* blurb)
{
	auto head = creditHeads.emplace(creditHeads.end());

	creditCount += (4 - creditCount%4) % 4;
	head->text = new TextElement(header, 30, &black);
	head->text->position(40, 250 + 60 + creditCount / 4 * 160);
	super::append(head->text);

	head->desc = new TextElement(blurb, 23, &gray, false, 1200);
	head->desc->position(40, 250 + 105 + creditCount / 4 * 160);
	super::append(head->desc);

	creditCount += 4;
}

void AboutScreen::credit(const char* username,
												const char* githubId,
												const char* twitter,
												const char* github,
												const char* gitlab,
												const char* patreon,
												const char* url,
												const char* discord,
												const char* directAvatarUrl)
{
	int X = 40;
	int Y = 310;

	int myX = creditCount % 4 * 300 + X;
	int myY = creditCount / 4 * 160 + Y;

	auto cred = credits.emplace(credits.end());

	auto avatar = directAvatarUrl ? directAvatarUrl : (std::string(AVATAR_URL) + githubId + "?s=100").c_str();
	cred->userLogo = new NetImageElement(directAvatarUrl != NULL ? directAvatarUrl : ((std::string(AVATAR_URL) + githubId + "?s=100").c_str()), [githubId]{
			return new ImageElement((std::string(RAMFS "res/pfp_cache/") + githubId).c_str());
		});
	cred->userLogo->position(myX, myY);
	cred->userLogo->resize(100, 100);
	super::append(cred->userLogo);

	cred->name = new TextElement(username, 27, &black);
	cred->name->position(myX + 110, myY);
	super::append(cred->name);

	int socialCount = 0;

	const char * handles[6] = { twitter, github, gitlab, patreon, url, discord };
	const char * icons[6] = { "twitter", "github", "gitlab", "patreon", "url", "discord" };

	for (int x=0; x<6; x++) {
		if (handles[x] == NULL) continue;

		cred->social[socialCount].icon = new ImageElement(((std::string(RAMFS "res/") + icons[x]) + ".png").c_str());
		cred->social[socialCount].icon->resize(20, 20);
		cred->social[socialCount].icon->position(myX + 110, myY + 45 + socialCount*25);
		super::append(cred->social[socialCount].icon);

		cred->social[socialCount].link = new TextElement(handles[x], 14, &gray);
		cred->social[socialCount].link->position(myX + 140, myY + 45 + socialCount*25);
		super::append(cred->social[socialCount].link);

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
	RootDisplay::switchSubscreen(nullptr);
}

void AboutScreen::launchFeedback()
{
	// find the package corresponding to us
	for (auto& package : this->get->packages)
	{
		if (package->pkg_name == "appstore")
		{
			RootDisplay::switchSubscreen(new Feedback(package));
			break;
		}
	}
}
