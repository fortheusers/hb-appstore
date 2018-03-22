#include "MainDisplay.hpp"

MainDisplay::MainDisplay()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL init failed: %s\n", SDL_GetError());
		return;
	}
	
	if (TTF_Init() < 0) {
		printf("SDL ttf init failed: %s\n", SDL_GetError());
		return;
	}
	
	printf("initialized SDL\n");
	
	this->window_surface = SDL_SetVideoMode(1280,720, 16, 0 /*SDL_FULLSCREEN*/);
	printf("got window surface\n");
	
	// create the first two elements (icon and app title)
//	ImageElement* icon = new ImageElement("icon.png");
//	icon->position(0.35, 0.5);
//	this->elements.push(icon);
	
	TextElement* title = new TextElement("Switch appstore", 26);
	title->position(0.4, 0.5);
	this->elements.push_back(title);
}

bool MainDisplay::processInput(SDL_Event* event)
{
	if (this->showingSplash)
	{
		// push A to dismiss splash screen
		if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_a)
		{
			this->showingSplash = false;
			
			
		}
	}
	else
	{
	}
	
	return false;
}

void MainDisplay::render(Element* parent)
{
	// set the background color
	background(0x42, 0x45, 0x48);

	// render the rest of the subelements
	super::render(this);
	
	// commit everything to the screen
	this->update();
}

void MainDisplay::background(int r, int g, int b)
{
	SDL_FillRect(this->window_surface, NULL, SDL_MapRGBA(this->window_surface->format, r, g, b, 0xFF));
}

void MainDisplay::update()
{
	SDL_Flip(this->window_surface);
}
