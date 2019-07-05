#include "AppBanner.hpp"
#include "MainDisplay.hpp"
#include <SDL2/SDL2_rotozoom.h>
#include <string.h>

AppBanner::AppBanner(Package *pkg)
{
#if defined(__WIIU__)
	loadIconBanner(pkg);
#else
	std::string url = *(pkg->repoUrl) + "/packages/" + pkg->pkg_name + "/screen.png";

	// try to find the banner in the caches first
	if (ImageCache::cache.count(url))
	{
		imgTexture = ImageCache::cache[url];
	}
	// not found
	else
	{
		// load icon banner
		loadIconBanner(pkg);

		// start banner download
		bannerDownload = new DownloadOperation();
		bannerDownload->url = url;
		bannerDownload->cb = std::bind(&AppBanner::bannerDownloadComplete, this, std::placeholders::_1);
		DownloadQueue::downloadQueue->downloadAdd(bannerDownload);
	}
#endif
}

AppBanner::~AppBanner()
{
	if (useIconBanner)
		delete icon;

	if (bannerDownload)
	{
		DownloadQueue::downloadQueue->downloadCancel(bannerDownload);
		delete bannerDownload;
	}

	SDL_DestroyTexture(imgTexture);
}

void AppBanner::loadIconBanner(Package *pkg)
{
	useIconBanner = true;

	// create app icon
	icon = new AppIcon(pkg);
	icon->height = height;
	icon->width = (256 * icon->height) / ICON_SIZE;
	icon->position(width / 2 - icon->width / 2, 0);
}

void AppBanner::bannerDownloadComplete(DownloadOperation *download)
{
	// the download failed, keep icon banner
	if (download->status == DownloadStatus::FAILED)
	{
		delete bannerDownload;
		bannerDownload = nullptr;
		return;
	}

	// load the image from the download buffer
	SDL_Surface *surface = IMG_Load_RW(SDL_RWFromMem((void*)download->buffer.c_str(), download->buffer.size()), 1);
	if (!surface)
	{
		delete bannerDownload;
		bannerDownload = nullptr;
		return;
	}
	imgTexture = SDL_CreateTextureFromSurface(MainDisplay::mainRenderer, surface);
	SDL_FreeSurface(surface);

	// save banner in caches for next time
	if (!ImageCache::cache.count(download->url))
		ImageCache::cache[download->url] = imgTexture;

	// the icon banner is no longer needed
	if (useIconBanner)
	{
		useIconBanner = false;
		delete icon;
	}

	needsRedraw = true;
}

void AppBanner::render(Element* parent)
{
	if (useIconBanner)
	{
		SDL_Rect banner_bg = { parent->x + x, parent->y + y, width, height };
		SDL_SetRenderDrawColor(parent->renderer, icon->firstPixel.r, icon->firstPixel.g, icon->firstPixel.b, 0xFF);
		SDL_RenderFillRect(parent->renderer, &banner_bg);
		icon->position(this->x + width / 2 - icon->width / 2, this->y);
		icon->render(parent);
		return;
	}
	else
	{
		SDL_Rect imgLocation;
		imgLocation.x = x + parent->x;
		imgLocation.y = y + parent->y;
		imgLocation.w = width;
		imgLocation.h = height;
		SDL_RenderCopy(MainDisplay::mainRenderer, imgTexture, NULL, &imgLocation);
	}
}

void AppBanner::resize(int width, int height)
{
	this->width = width;
	this->height = height;

	if (useIconBanner)
	{
		icon->height = height;
		icon->width = (256 * icon->height) / ICON_SIZE;
		icon->position(this->x + width / 2 - icon->width / 2, this->y);
	}
}

