#include "MainDisplay.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "../libs/chesto/src/Container.hpp"
#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/Constraint.hpp"
#include "SettingsScreen.hpp"
#include "ThemeManager.hpp"
#include <unordered_map>

void appendRow(Container* parent, Element* left, Element* right) {
    auto row = new Container(ROW_LAYOUT, 20);
    row->add(left);
    row->add(right);
    parent->add(row);
}

SettingsScreen::SettingsScreen()
{
    rebuildUI();
}

void SettingsScreen::rebuildUI() {
    removeAll();

    width = SCREEN_WIDTH;
    height = SCREEN_HEIGHT;

    auto color = HBAS::ThemeManager::background;
    this->backgroundColor = {color.r / 255.0, color.g / 255.0, color.b / 255.0};
    hasBackground = true;

    auto header = new TextElement(i18n("settings.title"), 30, &HBAS::ThemeManager::textPrimary);
    // header->constrain(ALIGN_TOP | ALIGN_CENTER_HORIZONTAL, 30);
    this->child(header);

    auto rows = new Container(COL_LAYOUT, 60);
    // main header

    // theme selection
    auto themeLabel = new TextElement(i18n("settings.theme.title"), 24, &HBAS::ThemeManager::textPrimary);
    auto choices = std::unordered_map<std::string, std::string> {
        { "auto", i18n("settings.auto") },
        { "light", i18n("settings.theme.light") },
        { "dark", i18n("settings.theme.dark") }
    };

    // resolution selection
    auto resLabel = new TextElement(i18n("settings.res.title"), 24, &HBAS::ThemeManager::textPrimary);
    auto resChoices = std::unordered_map<std::string, std::string> {
        // { "480p", i18n("settings.res.480p") },
        { "720p", i18n("settings.res.720p") },
        { "1080p", i18n("settings.res.1080p") },
        // { "4k", i18n("settings.res.4k") }
    };

    // language selection
    auto langLabel = new TextElement(i18n("settings.language"), 24, &HBAS::ThemeManager::textPrimary);

    // sidebar accent color
    auto accentLabel = new TextElement(i18n("settings.accentcolor"), 24, &HBAS::ThemeManager::textPrimary);
    auto accentChoices = std::unordered_map<std::string, std::string> {
        { "red", i18n("settings.accent.red") },
        { "orange", i18n("settings.accent.orange") },
        { "yellow", i18n("settings.accent.yellow") },
        { "green", i18n("settings.accent.green") },
        { "blue", i18n("settings.accent.blue") },
        { "purple", i18n("settings.accent.purple") },
        { "pink", i18n("settings.accent.pink") },
        { "lightgray", i18n("settings.accent.lightgray") },
        { "darkgray", i18n("settings.accent.darkgray") }
    };

    auto defaultTheme = "auto";
    if (HBAS::ThemeManager::preferredTheme != THEME_AUTO) {
        defaultTheme = (HBAS::ThemeManager::preferredTheme == THEME_LIGHT) ? "light" : "dark";
    }
    auto themeDropDown = new DropDown(this, R_BUTTON, choices, [this](std::string choice) {
        if (choice == "auto") {
            HBAS::ThemeManager::preferredTheme = THEME_AUTO;
        } else if (choice == "light") {
            HBAS::ThemeManager::preferredTheme = THEME_LIGHT;
        } else if (choice == "dark") {
            HBAS::ThemeManager::preferredTheme = THEME_DARK;
        }
        HBAS::ThemeManager::themeManagerInit(); // re-init theme
        Texture::wipeTextCache(); // all text elems need to be redrawn

        ((MainDisplay*)RootDisplay::mainDisplay)->rebuildUI(); // MainDisplay doesn't get reconstructed, so it needs to manually rebuild its subcomponents
        this->rebuildUI(); // andh our own UI too
    }, 20, defaultTheme);
    appendRow(rows, themeLabel, themeDropDown);

    auto resDropDown = new DropDown(this, X_BUTTON, resChoices, [this](std::string choice) {
        if (choice == "480p") {
            RootDisplay::mainDisplay->setScreenResolution(640, 480);
            std::cout << "Set resolution to 480p\n";
        }
        else if (choice == "720p") {
            RootDisplay::mainDisplay->setScreenResolution(1280, 720);
            std::cout << "Set resolution to 720p\n";
        } else if (choice == "1080p") {
            RootDisplay::mainDisplay->setScreenResolution(1920, 1080);
            std::cout << "Set resolution to 1080p\n";
        } else if (choice == "4k") {
            RootDisplay::mainDisplay->setScreenResolution(3840, 2160);
            std::cout << "Set resolution to 4k\n";
        }
        this->rebuildUI();
    }, 20, std::to_string(SCREEN_HEIGHT) + "p");
    appendRow(rows, resLabel, resDropDown);

     auto langDropdown = new DropDown(this, L_BUTTON, TextElement::i18nCache, [](std::string choice) {
        printf("Selected language: %s\n", choice.c_str());
    }, 20, "English");
    appendRow(rows, langLabel, langDropdown);
    
    appendRow(rows, accentLabel, new DropDown(this, Y_BUTTON, accentChoices, [](std::string choice) {
        // auto mainDisplay = (MainDisplay*)RootDisplay::mainDisplay;
        // mainDisplay->updateSidebarColor();
        printf("unimplemented: set accent color to %s\n", choice.c_str());
    }, 20, "Dark Gray"));

    // Back button in the corner
    auto backButton = new Button(i18n("settings.back"), B_BUTTON, false, 20);
    backButton->setAction([]() {
        RootDisplay::switchSubscreen(nullptr);
    });
    backButton->constrain(ALIGN_TOP | ALIGN_RIGHT,  20);
    this->child(backButton);

    // rows->constrain(OFFSET_TOP, 150)->constrain(OFFSET_LEFT, 100);
    this->child(rows);
}

SettingsScreen::~SettingsScreen()
{
}

bool SettingsScreen::process(InputEvents* event)
{
    if (curDropDown != nullptr) {
        // if a dropdown is open, pass events to it instead, and it will clean itself up when done
        return curDropDown->process(event);
    }
    super::process(event);
}

void SettingsScreen::render(Element* parent)
{
    super::render(parent);
}