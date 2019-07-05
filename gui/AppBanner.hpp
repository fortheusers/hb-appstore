#pragma once
#include <string>
#include "Element.hpp"
#include "AppIcon.hpp"
#include "ImageCache.hpp"
#include "libget/src/Package.hpp"

class AppBanner : public Element
{
public:
	AppBanner(Package *pkg);
	~AppBanner();

	void resize(int width, int height);
	void render(Element* parent);

private:
	void bannerDownloadComplete(DownloadOperation *download);
	DownloadOperation *bannerDownload = nullptr;

	void loadIconBanner(Package *pkg);

	bool useIconBanner = false;
	AppIcon *icon = nullptr;

	SDL_Texture* imgTexture = nullptr;
	SDL_Surface *imgSurface = nullptr;
};


