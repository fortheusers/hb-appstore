#include "Image.hpp"
#include "DownloadQueue.hpp"
#pragma once

class ImageNet : public Image
{
public:
	// Creates a new image element, downloading the image from url
	// If the url is not cached, getImageFallback will be called to
	// get an Image to be shown while downloading the correct image;
	// the provided Image is free'd when the download is complete
	// or the destructor is called
	ImageNet(std::string url, std::function<Image *(void)> getImageFallback);
	~ImageNet();

	void render(Element* parent);

private:
	void imgDownloadComplete(DownloadOperation *download);

	DownloadOperation *imgDownload = nullptr;
	Image *imgFallback = nullptr;
};
