#include "AppDetails.hpp"
#include "../libs/get/src/Get.hpp"
#include "../libs/get/src/Utils.hpp"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <sstream>
#include "MainDisplay.hpp"

AppDetails::AppDetails(Package* package, Get* get)
{
	this->package = package;
    this->get = get;

	SDL_Color red = {0xFF, 0x00, 0x00, 0xff};
	SDL_Color gray = {0x50, 0x50, 0x50, 0xff};
	SDL_Color black = {0x00, 0x00, 0x00, 0xff};
    SDL_Color white = {0xFF, 0xFF, 0xFF, 0xff};


	const char* action;
	switch (package->status)
	{
		case GET:
			action = "Download";
			break;
		case UPDATE:
			action = "Update";
			break;
		case INSTALLED:
			action = "Remove";
			break;
		default:
			action = "?";
	}

	// download/update/remove button (2)
    TextElement* download = new TextElement((std::string("(A) ") + action).c_str(), 30, &white);
	download->position(970, 570);
	this->elements.push_back(download);
    
    TextElement* cancel = new TextElement("(B) Cancel", 30, &white);
    cancel->position(970, 630);
    this->elements.push_back(cancel);


	TextElement* title = new TextElement(package->title.c_str(), 35, &black);
	title->position(20, 20);
	this->elements.push_back(title);

	int MARGIN = 525;

	TextElement* title2 = new TextElement(package->author.c_str(), 27, &gray);
	title2->position(20, 80);
	this->elements.push_back(title2);

	// the main description (wrapped text)
     TextElement* details = new TextElement(package->long_desc.c_str(), 20, &black, false, 700);
     details->position(150, 230);
     this->elements.push_back(details);
    
    
    // lots of details that we know about the package
    std::stringstream more_details;
    more_details << "Title: " << package->title << "\n"
    << package->short_desc << "\n\n"
    << "Author: " << package->author << "\n"
    << "Version: " << package->version << "\n"
    << "License: " << package->license << "\n\n"
    << "Package: " << package->pkg_name << "\n"
    << "Downloads: " << package->downloads << "\n"
    << "Updated: " << package->updated << "\n\n"
    << "Download size: " << package->download_size << " KB\n"
    << "Install size: " << package->extracted_size << " KB\n";
    
    auto mdeets = more_details.str();

    TextElement* more_details_elem = new TextElement(mdeets.c_str(), 20, &white, false, 300);
    more_details_elem->position(940, 50);
    this->elements.push_back(more_details_elem);

}

bool AppDetails::process(InputEvents* event)
{
	// don't process any keystrokes if an operation is in progress
	if (this->operating)
		return false;
    
    if (event->pressed(B_BUTTON))
    {
        MainDisplay::subscreen = NULL;
        return true;
    }

	if (this->highlighted >=0 && event->isKeyDown())
	{
		// update the highlight feature depending on left/right input
		if (event->held(LEFT_BUTTON))  this->highlighted = 0;
		if (event->held(RIGHT_BUTTON)) this->highlighted = 1;
	}

	// we need to detect if they hit download/update/remove or close
	// (this is not a great way to do this)
	// ((or A button was pressed))
//    if ((event->isTouchUp() && this->dragging) ||
//        (event->isKeyDown() && event->held(A_BUTTON)))
//    {
//        if (this->parent == NULL)
//            return false;
//
//        this->dragging = false;
//
//        int x = 570, y = 465;
//        int x2 = 950;
//        int w = 160, h = 55;

		// install/remove button pressed
		// (or we saw an A button pressed, and the first element is highlighted) (or just B)
//        if (event->touchIn(x, y, w, h) ||
//            (event->isKeyDown() && ((event->held(A_BUTTON) && this->highlighted == 0) || event->held(B_BUTTON))))
        if (event->pressed(A_BUTTON))
		{
			this->operating = true;
			// event->key.keysym.sym = SDLK_z;
			event->update();
			this->highlighted = -1;

			// add a progress bar to the screen to be drawn
			this->pbar = new ProgressBar();
			pbar->position(580, 495);
			pbar->color = 0xff0000ff;
			pbar->width = 500;
			this->elements.push_back(pbar);

			// hide the two specific elements for the download/install/remove and close buttons
//            this->elements[2]->hide();
//            this->elements[3]->hide();

			// setup progress bar callback
			networking_callback = AppDetails::updateCurrentlyDisplayedPopup;

			// install or remove this package based on the package status
            if (this->package->status == INSTALLED)
                get->remove(this->package);
            else
                get->install(this->package);

			// refresh the screen
			this->wipeElements();
			MainDisplay::subscreen = NULL;

			this->operating = false;
            return true;
		}

		// close button pressed
//        if (event->touchIn(x2, y, w, h) ||
//            (event->isKeyDown() && event->held(A_BUTTON) && this->highlighted == 1))
//        {
//            // remove elements on this pop up
//            this->wipeElements();
//
//            // our parent should also be AppList, tell it that the subscreen is dismissed
//            if (this->parent)
//            {
//                // refresh the screen
//                MainDisplay::subscreen = NULL;
//            }
//        }
//    }

	if (event->isTouchDown())
		this->dragging = true;

	return false;
}

void AppDetails::render(Element* parent)
{
	if (this->renderer == NULL)
		this->renderer = parent->renderer;
    if (this->parent == NULL)
        this->parent = parent;
    
    // draw white background
    SDL_Rect dimens = { 0, 0, 920, 720 };
    
    SDL_SetRenderDrawColor(parent->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(parent->renderer, &dimens);
    
    // draw all elements
    super::render(this);

	// draw the currently selected symbol, if appropriate
	if (this->highlighted >= 0)
	{
		int x = 570 + (this->highlighted)*380, y = 465;		// TODO: extract into formula (logic matches same check below in process)
		int w = 160, h = 55;

		if (this->package->status == GET && this->highlighted == 0)
			w += 25;

		rectangleRGBA(parent->renderer, x, y, x + w, y + h, 0xff, 0x00, 0xff, 0xff);
	}
}

int AppDetails::updateCurrentlyDisplayedPopup(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{    
    if (dltotal == 0) dltotal = 1;
    
    double amount = dlnow / dltotal;
    
    AppDetails* popup = (AppDetails*)MainDisplay::subscreen;

	// update the amount
	if (popup != NULL)
	{
		if (popup->pbar != NULL)
			popup->pbar->percent = amount;

		// force render the element right here (and it's progress bar too)
		if (popup->parent != NULL)
			popup->parent->render(NULL);

		// force update the main screen
		if (popup->renderer != NULL)
		{
            SDL_RenderPresent(popup->renderer);

			// must call poll event here to allow SDL to redraw the screen
			SDL_Event event;
			SDL_PollEvent(&event);
		}

	}
    
    return 0;
}
