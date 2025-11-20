#include "MainDisplay.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "../libs/chesto/src/Container.hpp"
#include "../libs/chesto/src/Button.hpp"
#include "../libs/chesto/src/Constraint.hpp"
#include "SettingsScreen.hpp"
#include "ThemeManager.hpp"
#include "AboutScreen.hpp"
#include "FeedbackCenter.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "main.hpp"

#include <vector>
#include <utility>
#include <fstream>
#include <sstream>

// put it in the .get folder
#define CONFIG_PATH DEFAULT_GET_HOME "settings.json"

void appendRow(Container* parent, Element* left, Element* right) {
    auto row = new Container(ROW_LAYOUT, 20);
    row->add(left);
    row->add(right);
    parent->add(row);
}

std::string SettingsScreen::getSettingsPath() {
    return CONFIG_PATH;
}

void SettingsScreen::saveSettings() {
    using namespace rapidjson;
    
    Document doc;
    doc.SetObject();
    Document::AllocatorType& allocator = doc.GetAllocator();
    
    // Save current settings
    Value themeVal;
    themeVal.SetString(currentTheme.c_str(), currentTheme.length(), allocator);
    doc.AddMember("theme", themeVal, allocator);
    
    // Value resVal;
    // resVal.SetString(currentResolution.c_str(), currentResolution.length(), allocator);
    // doc.AddMember("resolution", resVal, allocator);
    
    Value langVal;
    langVal.SetString(currentLanguage.c_str(), currentLanguage.length(), allocator);
    doc.AddMember("language", langVal, allocator);
    
    Value accentVal;
    accentVal.SetString(currentAccentColor.c_str(), currentAccentColor.length(), allocator);
    doc.AddMember("accentColor", accentVal, allocator);
    
    // write to output file
    StringBuffer buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    doc.Accept(writer);
    
    std::ofstream ofs(getSettingsPath());
    if (ofs.is_open()) {
        ofs << buffer.GetString();
        ofs.close();
        printf("Settings saved to %s\n", getSettingsPath().c_str());
    } else {
        printf("Failed to save settings to %s\n", getSettingsPath().c_str());
    }

    // now reload settings (all setting changes go through disk first)
    loadSettings();
}

void SettingsScreen::loadSettings() {
    using namespace rapidjson;
    
    // defaults
    currentTheme = "auto";
    // currentResolution = std::to_string(SCREEN_HEIGHT) + "p";
    currentLanguage = "English";
    currentAccentColor = "darkgray";
    
    // try to load from file
    std::ifstream ifs(getSettingsPath());
    if (!ifs.is_open()) {
        printf("No settings file found at %s, using defaults\n", getSettingsPath().c_str());
        return;
    }
    
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string content = buffer.str();
    ifs.close();
    
    Document doc;
    doc.Parse(content.c_str());
    
    if (doc.HasParseError()) {
        printf("Error parsing settings file, using defaults\n");
        return;
    }
    
    // Load settings
    if (doc.HasMember("theme") && doc["theme"].IsString()) {
        currentTheme = doc["theme"].GetString();
    }
    
    // if (doc.HasMember("resolution") && doc["resolution"].IsString()) {
    //     currentResolution = doc["resolution"].GetString();
    // }
    
    if (doc.HasMember("language") && doc["language"].IsString()) {
        currentLanguage = doc["language"].GetString();
    }
    
    if (doc.HasMember("accentColor") && doc["accentColor"].IsString()) {
        currentAccentColor = doc["accentColor"].GetString();
    }
    
    printf("Settings loaded from %s\n", getSettingsPath().c_str());
    
    // Apply loaded settings
    if (currentTheme == "auto") {
        HBAS::ThemeManager::preferredTheme = THEME_AUTO;
    } else if (currentTheme == "light") {
        HBAS::ThemeManager::preferredTheme = THEME_LIGHT;
    } else if (currentTheme == "dark") {
        HBAS::ThemeManager::preferredTheme = THEME_DARK;
    }
    
    // Apply accent color
    if (currentAccentColor == "red")
        HBAS::ThemeManager::setAccentColor(fromRGB(255, 0, 0));
    else if (currentAccentColor == "orange")
        HBAS::ThemeManager::setAccentColor(fromRGB(255, 165, 0));
    else if (currentAccentColor == "yellow")
        HBAS::ThemeManager::setAccentColor(fromRGB(255, 255, 0));
    else if (currentAccentColor == "green")
        HBAS::ThemeManager::setAccentColor(fromRGB(0, 255, 0));
    else if (currentAccentColor == "blue")
        HBAS::ThemeManager::setAccentColor(fromRGB(0, 0, 255));
    else if (currentAccentColor == "purple")
        HBAS::ThemeManager::setAccentColor(fromRGB(128, 0, 128));
    else if (currentAccentColor == "pink")
        HBAS::ThemeManager::setAccentColor(fromRGB(255, 192, 203));
    else if (currentAccentColor == "lightgray")
        HBAS::ThemeManager::setAccentColor(fromRGB(200, 200, 200));
    else if (currentAccentColor == "darkgray")
        HBAS::ThemeManager::setAccentColor(fromRGB(30, 30, 30));
}

SettingsScreen::SettingsScreen()
{
    loadSettings();
    rebuildUI();
}

void SettingsScreen::rebuildUI() {
    removeAll();

    // Main settings container - centered vertically and horizontally
    auto rows = new Container(COL_LAYOUT, 40);

    width = SCREEN_WIDTH; // pane is a fourth of the screen width
    height = SCREEN_HEIGHT;

    // Back button in the upper left corner
    auto backButton = new Button(i18n("settings.back"), B_BUTTON, true, 20);
    backButton->setAction([]() {
        RootDisplay::switchSubscreen(nullptr);
    });
    backButton->constrain(ALIGN_TOP | ALIGN_CENTER_HORIZONTAL, 20);
    rows->add(backButton);

    // Header centered at top
    auto header = new TextElement(i18n("settings.title"), 30, &HBAS::ThemeManager::textSecondary);
    header->position(0, 20);
    header->constrain(ALIGN_CENTER_HORIZONTAL, 0);
    rows->add(header);

    // Theme selection
    auto themeLabel = new TextElement(i18n("settings.theme.title"), 24, &HBAS::ThemeManager::textPrimary);
    auto choices = std::vector<std::pair<std::string, std::string>> {
        { "auto", i18n("settings.auto") },
        { "light", i18n("settings.theme.light") },
        { "dark", i18n("settings.theme.dark") }
    };

    auto themeDropDown = new DropDown(this, R_BUTTON, choices, [this](std::string choice) {
        currentTheme = choice;
        saveSettings();
        HBAS::ThemeManager::themeManagerInit();
        Texture::wipeTextCache();
        ((MainDisplay*)RootDisplay::mainDisplay)->rebuildUI();
        this->rebuildUI();
    }, 20, currentTheme, isDark);
    appendRow(rows, themeLabel, themeDropDown);

    // Resolution selection
    // auto resLabel = new TextElement(i18n("settings.res.title"), 24, &HBAS::ThemeManager::textPrimary);
    // auto resChoices = std::vector<std::pair<std::string, std::string>> {
    //     { "720p", i18n("settings.res.720p") },
    //     { "1080p", i18n("settings.res.1080p") },
    // };

    // auto resDropDown = new DropDown(this, X_BUTTON, resChoices, [this](std::string choice) {
    //     currentResolution = choice;
    //     saveSettings();
    //     this->rebuildUI();
    // }, 20, currentResolution, isDark);
    // appendRow(rows, resLabel, resDropDown);

    // Language selection
    auto langLabel = new TextElement(i18n("settings.language"), 24, &HBAS::ThemeManager::textPrimary);
    auto langDropdown = new DropDown(this, L_BUTTON, TextElement::getAvailableLanguages(), [this](std::string choice) {
        currentLanguage = choice;
        printf("Selected language: %s\n", choice.c_str());
        saveSettings();

        TextElement::loadI18nCache(choice);
        Texture::wipeTextCache();
        ((MainDisplay*)RootDisplay::mainDisplay)->rebuildUI();
        this->rebuildUI();

        ((MainDisplay*)RootDisplay::mainDisplay)->updateGetLocale(); // merge our repo data with the meta-repo's locale translation data

    }, 20, currentLanguage, isDark);
    appendRow(rows, langLabel, langDropdown);
    
    // Accent color selection
    auto accentLabel = new TextElement(i18n("settings.accentcolor"), 24, &HBAS::ThemeManager::textPrimary);
    auto accentChoices = std::vector<std::pair<std::string, std::string>> {
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
    
    appendRow(rows, accentLabel, new DropDown(this, Y_BUTTON, accentChoices, [this](std::string choice) {
        currentAccentColor = choice;
        saveSettings();
    }, 20, currentAccentColor, isDark));

    // Center the rows container
    this->child(rows);

    // Feedback button at bottom center
    auto feedbackCenter = new Button(i18n("listing.feedbackcenter"), 0, isDark, 20);
    feedbackCenter->setAction([]() {
        RootDisplay::switchSubscreen(new FeedbackCenter());
    });
    feedbackCenter->constrain(ALIGN_BOTTOM | ALIGN_CENTER_HORIZONTAL, 80);
    rows->add(feedbackCenter);

    // Credits button at bottom center (above feedback)
    auto credits = new Button(i18n("listing.credits"), 0, isDark, 20);
    credits->setAction([]() {
        RootDisplay::switchSubscreen(new AboutScreen(((MainDisplay*)RootDisplay::mainDisplay)->get));;
    });
    credits->constrain(ALIGN_BOTTOM | ALIGN_CENTER_HORIZONTAL, 20);
    rows->add(credits);

    // right align
    rows->height = SCREEN_HEIGHT;
    auto dest = 10; // 10 padding from the right
    // if we already transitioned in, don't redo it
    if (this->transitionAmt < 0x99) {
        rows->xAbs = rows->x = SCREEN_WIDTH*10;
        rows->animate(250, [rows, dest, this](float progress) {
            // std::cout << "Progress: " << progress << std::endl;
            rows->constraints.clear();
            rows->constrain(ALIGN_RIGHT, dest - (rows->width * (1 - progress)));
            // unless we've already faded in
            this->transitionAmt = static_cast<int>(0x99 * progress);
        }, [rows, dest, this]() {
            rows->constraints.clear();
            rows->constrain(ALIGN_RIGHT, dest);
            this->transitionAmt = 0x99;
        });
    } else {
        // just set the final constraint
        rows->constraints.clear();
        rows->constrain(ALIGN_RIGHT, dest);
    }
}

SettingsScreen::~SettingsScreen()
{
}

void SettingsScreen::render(Element* parent)
{
	if (this->parent == NULL)
		this->parent = parent;

    // draw a full screen dimmer background
    // TODO: this is probably literally the fourth place we do this, and we do it a different way every time
    CST_Rect fullDimens = { 0, 0, RootDisplay::screenWidth, RootDisplay::screenHeight };
    CST_SetDrawBlend(RootDisplay::renderer, true);
    CST_SetDrawColorRGBA(RootDisplay::renderer, 0, 0, 0, transitionAmt);
	CST_FillRect(RootDisplay::renderer, &fullDimens);
    CST_SetDrawBlend(RootDisplay::renderer, false);

    // draw the header bg, only over where the pane is
    auto row_x = elements[0]->xAbs;
    CST_Rect headerDimens = { row_x, 0, width, 85 };
    auto accentColor = RootDisplay::mainDisplay->backgroundColor;
    CST_SetDrawColor(RootDisplay::renderer, { accentColor.r * 0xff, accentColor.g * 0xff, accentColor.b * 0xff });
    CST_FillRect(RootDisplay::renderer, &headerDimens);

	// draw a white background, width of the screen
	CST_Rect dimens = { row_x, 85, width, height };

	CST_SetDrawColor(RootDisplay::renderer, HBAS::ThemeManager::background);
	CST_FillRect(RootDisplay::renderer, &dimens);

    // draw the secondary color along the top
    super::render(parent);
}