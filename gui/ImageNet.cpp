#include "ImageNet.hpp"
#include <SDL2/SDL_image.h>

ImageNet::ImageNet(std::string url, std::function<Image *(void)> getImageFallback)
{
	if (!loadFromCache(url))
	{
		// setup a temporary image fallback
		if (getImageFallback)
			imgFallback = getImageFallback();

		// start downloading the correct image
		imgDownload = new DownloadOperation();
		imgDownload->url = url;
		imgDownload->cb = std::bind(&ImageNet::imgDownloadComplete, this, std::placeholders::_1);
		DownloadQueue::downloadQueue->downloadAdd(imgDownload);
	}
}

ImageNet::~ImageNet()
{
	if (imgFallback)
		delete imgFallback;

	if (imgDownload)
	{
		DownloadQueue::downloadQueue->downloadCancel(imgDownload);
		delete imgDownload;
	}
}

void ImageNet::imgDownloadComplete(DownloadOperation *download)
{
	bool success = false;

	if (download->status == DownloadStatus::COMPLETE)
	{
		SDL_Surface *surface = IMG_Load_RW(SDL_RWFromMem((void*)download->buffer.c_str(), download->buffer.size()), 1);
		success = loadFromSurfaceSaveToCache(download->url, surface);
		SDL_FreeSurface(surface);
	}

	if (success)
	{
		this->needsRedraw = true;

		delete imgFallback;
		imgFallback = nullptr;
	}

	delete imgDownload;
	imgDownload = nullptr;
}


void ImageNet::render(Element* parent)
{
	if (imgTexture)
	{
		Image::render(parent);
	}
	else if (imgFallback)
	{
		imgFallback->x = x;
		imgFallback->y = y;
		imgFallback->width = width;
		imgFallback->height = height;
		imgFallback->render(parent);
	}
}
