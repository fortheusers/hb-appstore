#include "AppIcon.hpp"
#include "MainDisplay.hpp"
#include <SDL2/SDL.h>
#include <string>

AppIcon::AppIcon(Package *pkg)
{
	IconData iconData;
	std::string url = *(pkg->repoUrl) + "/packages/" + pkg->pkg_name + "/icon.png";
	std::string defaultIcon = std::string(ROMFS) + "res/default.png";

	// try to find it in the cache first
	if (ImageCache::iconCaches.count(url))
	{
		iconData = ImageCache::iconCaches[url];
	}
	// not found, create it
	else
	{
		// load temporary icon
		if (ImageCache::iconCaches.count(defaultIcon))
		{
			iconData = ImageCache::iconCaches[defaultIcon];
		}
		else
		{
			SDL_Surface *surface = IMG_Load(defaultIcon.c_str());
			if (surface)
			{
				iconData.texture = SDL_CreateTextureFromSurface(MainDisplay::mainRenderer, surface);
				Uint32 pixelcolor = 0;
				for (int i = 0; i < surface->format->BytesPerPixel; i++)
					pixelcolor = (pixelcolor << 8) + *((Uint8*)surface->pixels + i);
				SDL_GetRGB(pixelcolor, surface->format, &iconData.firstPixel.r, &iconData.firstPixel.g, &iconData.firstPixel.b);
				SDL_FreeSurface(surface);

				ImageCache::iconCaches[defaultIcon] = iconData;
			}
		}

		// start icon download
		iconDownload = new DownloadOperation();
		iconDownload->url = url;
		iconDownload->cb = std::bind(&AppIcon::iconDownloadComplete, this, std::placeholders::_1);
		DownloadQueue::downloadQueue->downloadAdd(iconDownload);
	}

	imgTexture = iconData.texture;
	firstPixel = iconData.firstPixel;
}

AppIcon::~AppIcon()
{
	if (iconDownload)
	{
		DownloadQueue::downloadQueue->downloadCancel(iconDownload);
		delete iconDownload;
	}
}

void AppIcon::iconDownloadComplete(DownloadOperation *download)
{
	IconData iconData;

	// the download failed, keep default icon
	if (download->status == DownloadStatus::FAILED)
	{
		delete iconDownload;
		iconDownload = nullptr;
		return;
	}

	// load the image from the download buffer
	SDL_Surface *surface = IMG_Load_RW(SDL_RWFromMem((void*)download->buffer.c_str(), download->buffer.size()), 1);
	if (!surface)
	{
		delete iconDownload;
		iconDownload = nullptr;
		return;
	}

	// create icon data for the new image
	iconData.texture = SDL_CreateTextureFromSurface(MainDisplay::mainRenderer, surface);
	Uint32 pixelcolor = 0;
	for (int i = 0; i < surface->format->BytesPerPixel; i++)
		pixelcolor = (pixelcolor << 8) + *((Uint8*)surface->pixels + i);
	SDL_GetRGB(pixelcolor, surface->format, &iconData.firstPixel.r, &iconData.firstPixel.g, &iconData.firstPixel.b);
	SDL_FreeSurface(surface);

	// save icon in caches for next time
	if (!ImageCache::iconCaches.count(download->url))
		ImageCache::iconCaches[download->url] = iconData;

	// apply new icon
	imgTexture = iconData.texture;
	firstPixel = iconData.firstPixel;

	needsRedraw = true;
}

void AppIcon::render(Element* parent)
{
	SDL_Rect imgLocation;
	imgLocation.x = x + parent->x;
	imgLocation.y = y + parent->y;
	imgLocation.w = width;
	imgLocation.h = height;

	SDL_RenderCopy(MainDisplay::mainRenderer, imgTexture, NULL, &imgLocation);
}

void AppIcon::resize(int width, int height)
{
	this->width = width;
	this->height = height;
}
