#include "../libs/chesto/src/RootDisplay.hpp"
#include "../libs/chesto/src/DropDown.hpp"
#include <string>

class SettingsScreen : public DropDownControllerElement
{
public:
    SettingsScreen();
    ~SettingsScreen();
    void render(Element* parent);
    void rebuildUI();
    
    // settings persistence
    void saveSettings();
    void loadSettings();
    std::string getSettingsPath();
    
    // current settings values
    std::string currentTheme;
    std::string currentResolution;
    std::string currentLanguage;
    std::string currentAccentColor;
};