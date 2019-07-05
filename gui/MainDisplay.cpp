#if defined(SWITCH)
#include <switch.h>
#define PLATFORM "Switch"
#elif defined(__WIIU__)
#include <romfs-wiiu.h>
#define PLATFORM "Wii U"
#else
#define PLATFORM "Console"
#endif

#include "libget/src/Utils.hpp"

#include "MainDisplay.hpp"
#include "AppCard.hpp"
#include "Button.hpp"

SDL_Renderer* MainDisplay::mainRenderer = NULL;
Element* MainDisplay::subscreen = NULL;
MainDisplay* MainDisplay::mainDisplay = NULL;

MainDisplay::MainDisplay(Get* get)
{
// initialize romfs for switch/wiiu
#if defined(SWITCH) || defined(__WIIU__)
	romfsInit();
#endif

	this->get = get;

	// populate image cache with any local version info if it exists
	this->imageCache = new ImageCache(get->tmp_path);

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0)
	{
		//        printf("SDL init failed: %s\n", SDL_GetError());
		return;
	}

	// use linear filtering when available
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

//Initialize SDL_mixer
#if defined(MUSIC)
	Mix_Init(MIX_INIT_MP3);
	Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
	this->music = Mix_LoadMUS(ROMFS "./res/music.mp3");
	if (music)
	{
		Mix_FadeInMusic(music, -1, 300);
	}
#endif

	if (TTF_Init() < 0)
	{
		//        printf("SDL ttf init failed: %s\n", SDL_GetError());
		return;
	}

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		//        printf("SDL image init failed: %s\n", SDL_GetError());
		return;
	}

	//    printf("initialized SDL\n");

	int height = 720;
	int width = 1280;

	this->window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
	this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);

	//Detach the texture
	SDL_SetRenderTarget(this->renderer, NULL);

	MainDisplay::mainRenderer = this->renderer;
	MainDisplay::mainDisplay = this;

	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		if (SDL_JoystickOpen(i) == NULL)
		{
			//                printf("SDL_JoystickOpen: %s\n", SDL_GetError());
			SDL_Quit();
			return;
		}
	}

	// set up the SDL needsRender event
	this->needsRender.type = SDL_USEREVENT;

	// go through all repos and if one has an error, set the error flag
	bool atLeastOneEnabled = false;
	for (auto repo : this->get->repos)
	{
		this->error = this->error || !repo->loaded;
		atLeastOneEnabled = atLeastOneEnabled || repo->enabled;
	}

	this->error = this->error || !atLeastOneEnabled;

	// the text above the progress bar
	//	TextElement* pbar_text = new TextElement("Updating App Info...", 17);
	//	pbar_text->position(550, 365);
	//	this->elements.push_back(pbar_text);

	if (this->error)
	{
		std::string troubleshootingText = "No enabled repos found, check ./get/repos.json\nMake sure repo has at least one package";
		if (atLeastOneEnabled)
			troubleshootingText = std::string("Perform a connection test in the " PLATFORM " System Settings\nEnsure DNS isn't blocking: ") + this->get->repos[0]->url;

		TextElement* errorMessage = new TextElement("Couldn't connect to the Internet!", 40);
		errorMessage->position(345, 305);
		this->elements.push_back(errorMessage);

		TextElement* troubleshooting = new TextElement((std::string("Troubleshooting:\n") + troubleshootingText).c_str(), 20, NULL, false, 600);
		troubleshooting->position(380, 585);
		this->elements.push_back(troubleshooting);

		Button* quit = new Button("Quit", SELECT_BUTTON, false, 15);
		quit->position(1130, 630);
		quit->action = std::bind(&MainDisplay::exit, this);
		this->elements.push_back(quit);

		return;
	}

	// add in the sidebar, footer, and main app listing
	Sidebar* sidebar = new Sidebar();
	this->elements.push_back(sidebar);

	AppList* applist = new AppList(this->get, sidebar);
	this->elements.push_back(applist);
	sidebar->appList = applist;

	this->needsRedraw = true;
}

bool MainDisplay::process(InputEvents* event)
{
	if (MainDisplay::subscreen)
		return MainDisplay::subscreen->process(event);
	// keep processing child elements
	return super::process(event);
}

void MainDisplay::render(Element* parent)
{
	// set the background color
	MainDisplay::background(0x42, 0x45, 0x48);
//    MainDisplay::background(0x60, 0x7d, 0x8b);
#if defined(__WIIU__)
	MainDisplay::background(0x54, 0x55, 0x6e);
#endif

	if (MainDisplay::subscreen)
	{
		MainDisplay::subscreen->render(this);
		this->update();
		return;
	}

	// render the rest of the subelements
	super::render(this);

	// commit everything to the screen
	this->update();
}

void MainDisplay::background(int r, int g, int b)
{
	SDL_SetRenderDrawColor(this->renderer, r, g, b, 0xFF);
	SDL_RenderFillRect(this->renderer, NULL);
}

void MainDisplay::update()
{
	// never exceed 60fps because there's no point

	//    int now = SDL_GetTicks();
	//    int diff = now - this->lastFrameTime;
	//
	//    if (diff < 16)
	//        return;

	SDL_RenderPresent(this->renderer);
	//    this->lastFrameTime = now;
}

void MainDisplay::exit()
{
	quit();
}

void quit()
{
	DownloadQueue::quit();

	IMG_Quit();
	TTF_Quit();

	SDL_Delay(10);
	SDL_DestroyWindow(MainDisplay::mainDisplay->window);

	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();

#if defined(__WIIU__)
	romfsExit();
#endif

#if defined(SWITCH)
	socketExit();
#endif
	exit(0);
}
