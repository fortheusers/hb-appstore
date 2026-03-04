#include "../libs/chesto/src/RootDisplay.hpp"
#include "../libs/chesto/src/DropDown.hpp"
#include <string>

using namespace Chesto;

class SettingsScreen : public Screen
{
public:
    SettingsScreen();
    ~SettingsScreen();
    void render(Element* parent) override;
    bool process(InputEvents* event) override;
    void rebuildUI() override;
    
    // settings persistence
    void saveSettings();
    void loadSettings();
    std::string getSettingsPath();
    
    static void loadSettingsStatic();
    
    // current settings values
    std::string currentTheme;
    std::string currentResolution;
    std::string currentLanguage;
    std::string currentAccentColor;

    int transitionAmt = 0; // how far in we've faded in
    bool needsRebuild = false; // flag to rebuild on next frame
};