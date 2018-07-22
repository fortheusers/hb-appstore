#include "TextElement.hpp"
#include "ImageElement.hpp"

class AppList;

#define TOTAL_CATS 7

class Sidebar : public Element
{
public:
	Sidebar();

	std::string currentCatName();
	std::string currentCatValue();

	std::string searchQuery = "vgmoose";

	AppList* appList = NULL;
	int highlighted = -1;

	void render(Element* parent);
	bool process(InputEvents* event);

	// the currently selected category index
	int curCategory = 1;		// 1 is all apps

	// list of human-readable category titles and short names from the json
	const char* cat_names[TOTAL_CATS] = {"Search", "All Apps", "Games", "Emulators", "Tools", "Loaders", "Concepts"};
	const char* cat_value[TOTAL_CATS] = {"_search", "*", "game", "emu", "tool", "loader", "concept"};

};
