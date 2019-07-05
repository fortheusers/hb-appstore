#pragma once
#include <string>
#include "Element.hpp"
#include "ImageCache.hpp"
#include "DownloadQueue.hpp"
#include "libget/src/Package.hpp"

class AppIcon : public Element
{
public:
	AppIcon(Package *pkg);
	virtual ~AppIcon();

	void render(Element* parent);
	void resize(int width, int height);

	SDL_Color firstPixel = {0,0,0,0};

private:
	void iconDownloadComplete(DownloadOperation *download);
	DownloadOperation *iconDownload = nullptr;

	SDL_Texture* imgTexture = nullptr;
};

