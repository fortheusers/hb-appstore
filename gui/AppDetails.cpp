#include "AppDetails.hpp"
#include "../libs/get/src/Get.hpp"
#include "../libs/get/src/Utils.hpp"
#include "Button.hpp"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <sstream>
#include "MainDisplay.hpp"
#include "Feedback.hpp"

AppDetails::AppDetails(Package* package, AppList* appList)
{
	this->package = package;
    this->get = appList->get;
    this->appList = appList;

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
        case LOCAL:
            action = "Reinstall";
            break;
		default:
			action = "?";
	}

    // TODO: show current app status somewhere

	// download/update/remove button (2)

    Button* download = new Button(action, A_BUTTON, true, 30);
	download->position(970, 550);
    download->action = std::bind(&AppDetails::proceed, this);
	this->elements.push_back(download);

    Button* cancel = new Button("Cancel", B_BUTTON, true, 30, download->width);
    cancel->position(970, 630);
    cancel->action = std::bind(&AppDetails::back, this);
    this->elements.push_back(cancel);


    // the scrollable portion of the app details page
    AppDetailsContent* content = new AppDetailsContent();
    this->elements.push_back(content);

    int MARGIN = 60;

	TextElement* title = new TextElement(package->title.c_str(), 35, &black);
	title->position(MARGIN, 30);
	content->elements.push_back(title);

    Button* moreByAuthor = new Button("More by Author", X_BUTTON);

    Button* reportIssue = new Button("Report Issue", Y_BUTTON);
    reportIssue->position(920 - MARGIN - reportIssue->width, 45);
    moreByAuthor->position(reportIssue->x - 20 - moreByAuthor->width, 45);
    moreByAuthor->action = std::bind(&AppDetails::moreByAuthor, this);
    reportIssue->action = std::bind(&AppDetails::leaveFeedback, this);
    content->elements.push_back(reportIssue);
    content->elements.push_back(moreByAuthor);

    ImageElement* banner = new ImageElement((ImageCache::cache_path + package->pkg_name + "/screen.png").c_str());
    // TODO: check for missing banner and show nothing, and shift everything up in that case
//    banner->resize(848, 208);
//    banner->resize(727, 179);
    banner->resize(787, 193);

    banner->position(MARGIN + 5, 140);
    content->elements.push_back(banner);

	TextElement* title2 = new TextElement(package->author.c_str(), 27, &gray);
	title2->position(MARGIN, 80);
	content->elements.push_back(title2);

	// the main description (wrapped text)
     TextElement* details = new TextElement(package->long_desc.c_str(), 20, &black, false, 740);
     details->position(MARGIN + 30, 355);
     content->elements.push_back(details);
    
    TextElement* changelog = new TextElement((std::string("Changelog:\n") + package->changelog).c_str(), 20, &black, false, 740);
    changelog->position(MARGIN + 30, details->y + details->height + 30);
    content->elements.push_back(changelog);


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

// TODO: make one push event function to bind instead of X separeate ones
void AppDetails::proceed()
{
    SDL_Event sdlevent;
    sdlevent.type = SDL_KEYDOWN;
    sdlevent.key.keysym.sym = SDLK_a;
    SDL_PushEvent(&sdlevent);
}

void AppDetails::back()
{
    SDL_Event sdlevent;
    sdlevent.type = SDL_KEYDOWN;
    sdlevent.key.keysym.sym = SDLK_b;
    SDL_PushEvent(&sdlevent);
}

void AppDetails::moreByAuthor()
{
    const char* author = this->package->author.c_str();
    appList->sidebar->searchQuery = std::string(author);
    appList->sidebar->curCategory = 0;
    appList->update();
    appList->y = 0;
    appList->keyboard->hidden = true;
    MainDisplay::subscreen = NULL;      // TODO: clean up memory?
}

void AppDetails::leaveFeedback()
{
    MainDisplay::subscreen = new Feedback(this->package);
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

        if (event->pressed(A_BUTTON))
		{
			this->operating = true;
			// event->key.keysym.sym = SDLK_z;
			event->update();
			this->highlighted = -1;

			// add a progress bar to the screen to be drawn
			this->pbar = new ProgressBar();
            pbar->width = 740;
			pbar->position(1280/2 - this->pbar->width/2, 720/2 - 5);
			pbar->color = 0xff0000ff;
            pbar->dimBg = true;
			this->elements.push_back(pbar);

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
            this->appList->update();
            return true;
		}

	if (event->isTouchDown())
		this->dragging = true;

    return super::process(event);
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

void AppDetailsContent::render(Element* parent)
{
    if (this->parent == NULL)
        this->parent = parent;

    this->renderer = parent->renderer;

    super::render(this);
}

bool AppDetailsContent::process(InputEvents* event)
{
    return ListElement::process(event);
}
