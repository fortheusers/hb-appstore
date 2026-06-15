#pragma once

#include "../libs/chesto/src/Screen.hpp"
#include "../libs/get/src/Get.hpp"
#include "AppList.hpp"
#include "Sidebar.hpp"

using namespace Chesto;

class AppListScreen : public Screen
{
public:
    AppListScreen(Get* get);
    ~AppListScreen();
    
    void render(Element* parent) override;
    bool process(InputEvents* event) override;
    
    void rebuild(); // TODO: use rebuildUI instead?
    
    std::string getCurrentCategory() const { return currentCategory; }
    std::string getSearchQuery() const { return searchQuery; }
    int getSortMode() const { return sortMode; }
    
    void setCategory(const std::string& cat);
    void setSearchQuery(const std::string& query);
    void setSortMode(int mode);
    
private:
    std::string currentCategory = "_all";
    std::string searchQuery = "";
    int sortMode = RECENT;
    
    Sidebar* sidebar = nullptr;
    AppList* appList = nullptr;
    
    void syncStateToComponents();
};
