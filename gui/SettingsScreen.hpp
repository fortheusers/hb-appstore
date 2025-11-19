#include "../libs/chesto/src/RootDisplay.hpp"
#include "../libs/chesto/src/DropDown.hpp"

class SettingsScreen : public DropDownControllerElement
{
public:
    SettingsScreen();
    ~SettingsScreen();
    void render(Element* parent);
    void rebuildUI();
};