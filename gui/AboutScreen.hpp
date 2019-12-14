#ifndef ABOUTSCREEN_H_
#define ABOUTSCREEN_H_

#include "../libs/get/src/Get.hpp"

#include "../libs/chesto/src/ImageElement.hpp"
#include "../libs/chesto/src/ListElement.hpp"
#include "../libs/chesto/src/TextElement.hpp"

class AboutScreen : public ListElement
{
public:
	AboutScreen(Get* get);
	Get* get = NULL;
	void render(Element* parent);

	// button bindings
	void back();
	void removeEmptyFolders();
	void wipeCache();
	void launchFeedback();
  void credHead(const char* header, const char* blurb);
  void credit(const char* username,
              const char* githubId,
              const char* twitter = NULL,
              const char* github = NULL,
              const char* gitlab = NULL,
              const char* patreon = NULL,
              const char* url = NULL,
              const char* discord = NULL,
              const char* directAvatarUrl = NULL);

  int creditCount = 0;
};

#endif
