#include "AppList.hpp"

Sidebar::Sidebar()
{
	// a sidebar consists of:
	//		a collection of category labels (TextElements),
	//		and an image (ImageElement) and a logo (TextElement)
	
	// it also can process input if the cursor goes over it, or a touch
	
	// there's no back color to the sidebar, as the background is already the right color
	
	// create image in top left
	ImageElement* logo = new ImageElement("res/icon_small.png");
	logo->resize(35, 35);
	logo->position(50, 50);
	this->elements.push_back(logo);
	
	// create title for logo, top left
	TextElement* title = new TextElement("Switch appstore", 27);
	title->position(90, 50);
	this->elements.push_back(title);
	
	// for every entry in cat names, create a text element
	for (int x=0; x<TOTAL_CATS; x++)
	{
		TextElement* category = new TextElement(cat_names[x], 25);
		category->position(50, 150+x*70);
		this->elements.push_back(category);
	}
}

bool Sidebar::process(SDL_Event* event)
{
	// saw click down, set dragging state
	if (event->type == SDL_MOUSEBUTTONDOWN)
	{
		this->dragging = true;
		return false;
	}
	
	// detect if a click is on one of the sidebar elements
	if (event->type == SDL_MOUSEBUTTONUP && this->dragging)
	{
		this->dragging = false;
		
		// check if it's one of the text elements
		for (int x=0; x<TOTAL_CATS; x++)
		{
			int xc = 0, yc = 150+x*70 - 15, width = 400, height = 60;		// TODO: extract formula into method (same as below)
			if (event->motion.x >= xc &&
				event->motion.x < xc + width &&
				event->motion.y >= yc &&
				event->motion.y < yc + height)
			{
				// saw touchup on a category, adjust active category
				this->curCategory = x;
				
				// since we updated the active category, we need to update the app listing
				if (this->appList != NULL)
				{
					this->appList->y = 0;
					this->appList->update();
				}
			}
		}
	}
	
	return false;
}

void Sidebar::render(Element* parent)
{
	// draw the light gray bg behind the active category
	SDL_Rect dimens = { 0, 0, 400, 60 };
	dimens.y = 150+this->curCategory*70 - 15;		// TODO: extract formula into method
	
	SDL_FillRect(parent->window_surface, &dimens, SDL_MapRGBA(parent->window_surface->format, 0x67, 0x6a, 0x6d, 0xFF));
	
	// render subelements
	super::render(parent);
}

std::string Sidebar::currentCatName()
{
	if (this->curCategory >= 0 && this->curCategory < TOTAL_CATS)
		return std::string(this->cat_names[this->curCategory]);
	
	return std::string("?");
}

std::string Sidebar::currentCatValue()
{
	if (this->curCategory >= 0 && this->curCategory < TOTAL_CATS)
		return std::string(this->cat_value[this->curCategory]);
	
	return std::string("?");
}
