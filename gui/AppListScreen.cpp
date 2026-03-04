#include "AppListScreen.hpp"
#include "MainDisplay.hpp"

AppListScreen::AppListScreen(Get* get)
{
    sidebar = createNode<Sidebar>();
    appList = createNode<AppList>(get, sidebar);
    
    // initial state
    currentCategory = "_all";
    searchQuery = "";
    sortMode = RECENT; // default
    
    syncStateToComponents();
}

AppListScreen::~AppListScreen()
{
}

void AppListScreen::render(Element* parent)
{
    if (this->parent == nullptr)
        this->parent = parent;
    
    Screen::render(parent);
}

bool AppListScreen::process(InputEvents* event)
{
    // TODO: manage focus state here between sidebar and appList
    return Screen::process(event);
}

void AppListScreen::rebuild()
{
    if (appList) {
        appList->needsUpdate = true;
    }
}

void AppListScreen::setCategory(const std::string& cat)
{
    if (currentCategory != cat) {
        currentCategory = cat;
        syncStateToComponents();
        rebuild();
    }
}

void AppListScreen::setSearchQuery(const std::string& query)
{
    if (searchQuery != query) {
        searchQuery = query;
        syncStateToComponents();
        rebuild();
    }
}

void AppListScreen::setSortMode(int mode)
{
    if (sortMode != mode) {
        sortMode = mode;
        syncStateToComponents();
        rebuild();
    }
}

void AppListScreen::syncStateToComponents()
{
    if (appList) {
        appList->sortMode = sortMode;
    }
}
