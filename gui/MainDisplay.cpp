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
#include "chesto/src/Button.hpp"

#include "MainDisplay.hpp"
#include "AppCard.hpp"

MainDisplay::MainDisplay(Get* get)
{
	RootDisplay::super();

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

	// the progress bar
	ProgressBar* pbar = new ProgressBar();
	pbar->position(401, 380 - this->error * 290);
	this->elements.push_back(pbar);

	// the text above the progress bar
	//	TextElement* pbar_text = new TextElement("Updating App Info...", 17);
	//	pbar_text->position(550, 365);
	//	this->elements.push_back(pbar_text);

	// create the first two elements (icon and app title)
	ImageElement* icon = new ImageElement(ROMFS "res/icon.png");
	icon->position(330 + this->error * 140, 255 - this->error * 230);
	icon->resize(70 - this->error * 35, 70 - this->error * 35);
	this->elements.push_back(icon);

	TextElement* title = new TextElement("Homebrew App Store", 50 - this->error * 25);
	title->position(415 + this->error * 100, 255 - this->error * 230);
	this->elements.push_back(title);

	if (this->imageCache->version_cache.size() == 0)
	{
		notice = new TextElement("Still doing initial load-- next time will be faster!", 20);
		notice->position(410, 460);
		notice->hidden = true;
		this->elements.push_back(notice);
	}

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
	}
}