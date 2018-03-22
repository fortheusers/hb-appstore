#include "MainDisplay.hpp"

MainDisplay::MainDisplay()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL init failed: %s\n", SDL_GetError());
		return;
	}
	
	printf("initialized SDL\n");
	
	this->window_surface = SDL_SetVideoMode(1280,720, 16, 0 /*SDL_FULLSCREEN*/);
	printf("got window surface\n");
}

bool MainDisplay::processInput(SDL_Event* event)
{
	if (this->showingSplash)
	{
		
	}
	else
	{
	}
	
	return false;
}

void MainDisplay::render()
{
	// set the background color
	background(0x42, 0x45, 0x48);

	// render the rest of the subelements
	super::render();
	
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
