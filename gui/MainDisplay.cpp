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
	
	int imgFlags = IMG_INIT_PNG;
	if( !( IMG_Init( imgFlags ) & imgFlags ) )
	{
		printf("SDL image init failed: %s\n", SDL_GetError());
		return;
	}
	
	printf("initialized SDL\n");
	
	int height = 720;
	int width = 1280;
	
	this->window_surface = SDL_SetVideoMode(width, height, 16, 0 /*SDL_FULLSCREEN*/);
	printf("got window surface\n");
	
	// the progress bar
	ProgressBar* pbar = new ProgressBar();
	pbar->position(401, 380);
	this->elements.push_back(pbar);
	
	// the text above the progress bar
//	TextElement* pbar_text = new TextElement("Updating App Info...", 17);
//	pbar_text->position(550, 365);
//	this->elements.push_back(pbar_text);
	
	// create the first two elements (icon and app title)
	ImageElement* icon = new ImageElement("res/icon.png");
	icon->position(410, 250);
	this->elements.push_back(icon);
	
	TextElement* title = new TextElement("Switch appstore", 50);
	title->position(485, 255);
	this->elements.push_back(title);
}

bool MainDisplay::process(SDL_Event* event)
{
	if (this->showingSplash)
	{
		// update the counter (TODO: replace with fetching app icons/screen previews)
		this->count++;
		
		// should be a progress bar
		((ProgressBar*)this->elements[0])->percent = (this->count / 50.0f);
		
		if (this->count == 51)	//TODO: replace with sum of apps*2
		{
			// remove the splash screen elements
			this->wipeElements();
			
			// add in the sidebar, footer, and main app listing
			Sidebar* sidebar = new Sidebar();
			this->elements.push_back(sidebar);
			
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
