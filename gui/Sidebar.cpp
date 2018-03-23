#include "Sidebar.hpp"

Sidebar::Sidebar()
{
	const char* cat_names[] = {"All Apps", "Games", "Emulators", "Tools", "Loaders", "Concepts"};
	const char* cat_value[] = {"*", "game", "emu", "tool", "loader", "concept"};
	
	// a sidebar consists of:
	//		a collection of category labels (TextElements),
	//		and an image (ImageElement) and a logo (TextElement)
	
	// it also can process input if the cursor goes over it, or a touch
	
	// there's no back color to the sidebar, as the background is already the right color
	
	// create image in top left
	ImageElement* logo = new ImageElement("res/icon_small.png");
	logo->resize(35, 35);
	logo->position(85, 50);
	this->elements.push_back(logo);
	
	// create title for logo, top left
	TextElement* title = new TextElement("Switch appstore", 27);
	title->position(125, 50);
	this->elements.push_back(title);
}
