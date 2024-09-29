#include "../libs/chesto/src/RootDisplay.hpp"

class SettingsScreen : public Element
{
public:
    SettingsScreen();
    ~SettingsScreen();
    bool process(InputEvents* event);
    void render(Element* parent);
};