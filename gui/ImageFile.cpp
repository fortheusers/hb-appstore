#include "ImageFile.hpp"
#include <SDL2/SDL_image.h>

ImageFile::ImageFile(std::string path)
{
	if (!loadFromCache(path))
	{
		SDL_Surface *surface = IMG_Load(path.c_str());
		loadFromSurfaceSaveToCache(path, surface);
		SDL_FreeSurface(surface);
	}
}
