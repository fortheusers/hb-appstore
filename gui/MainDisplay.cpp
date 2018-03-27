#include "MainDisplay.hpp"
#include "AppCard.hpp"
#include "../libs/get/src/Utils.hpp"

MainDisplay::MainDisplay(Get* get)
{
	this->get = get;
	
	// populate image cache with any local version info if it exists
	this->imageCache = new ImageCache(get->tmp_path);
	
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
	
	#if defined(SWITCH)
		// hide cursor for switch
		SDL_ShowCursor(0);
	#endif
	
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
	// if we're on the splash/loading screen, we need to fetch icons+screenshots from the remote repo
	// and load them into our surface cache with the pkg_name+version as the key
	
	if (this->showingSplash)
	{
		// update the counter (TODO: replace with fetching app icons/screen previews)
		this->count++;
		
		// should be a progress bar
		if (this->get->packages.size() != 1)
			((ProgressBar*)this->elements[0])->percent = (this->count / ((float)this->get->packages.size()-1));
		
		// get the package whose icon+screen to process
		Package* current = this->get->packages[this->count - 1];
		
		// the path to the cache location of the icon and screen for this pkg_name and version number
		std::string key_path = imageCache->cache_path + current->pkg_name;
		
		// check if this package exists in our cache, but the version doesn't match
		// (if (it's not in the cache) OR (it's in the cache but the version doesn't match)
		if (this->imageCache->version_cache.count(current->pkg_name) == 0 ||
			(this->imageCache->version_cache.count(current->pkg_name) &&
			 this->imageCache->version_cache[current->pkg_name] != current->version))
		{
			// the version in our cache doesn't match the one that will be on the server
			// so we need to download the images now
			mkdir(key_path.c_str(), 0700);
			
			bool success = downloadFileToDisk(*(current->repoUrl) + "/packages/" + current->pkg_name + "/icon.png", key_path + "/icon.png");
			if (!success) // manually add defualt icon to cache if downloading failed
				cp("res/default.png", (key_path + "/icon.png").c_str());
			
			success = downloadFileToDisk(*(current->repoUrl) + "/packages/" + current->pkg_name + "/screen.png", key_path + "/screen.png");
			if (!success)
				cp("res/noscreen.png", (key_path + "/screen.png").c_str());

			// add these versions to the version map
			this->imageCache->version_cache[current->pkg_name] = current->version;
		}
		
		// whether we just downloaded it or it was already there from the cache, load this image element into our memory cache
		// (making an AppCard and calling update() will do this, even if we don't intend to do anything with it yet)
		AppCard a(current);
		a.update();
		
		// write the version we just got to the cache as well so that we can know whether or not we need to up date it next time
		
		// are we done processing all packages
		if (this->count == this->get->packages.size())
		{
			// write whatever we have in the icon version cache to a file
			this->imageCache->writeVersionCache();
			
			// remove the splash screen elements
			this->wipeElements();
			
			// add in the sidebar, footer, and main app listing
			Sidebar* sidebar = new Sidebar();
			this->elements.push_back(sidebar);
			AppList* applist = new AppList(this->get);
			this->elements.push_back(applist);
			
			this->showingSplash = false;
		}
	}
	else
	{
		// keep processing child elements
		super::process(event);
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
