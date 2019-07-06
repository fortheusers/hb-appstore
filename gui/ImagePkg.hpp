 #include "libget/src/Package.hpp"
 #include "ImageNet.hpp"
 #include "ImageFile.hpp"
 #include "Image.hpp"
 #pragma once

namespace ImagePkg
{
	Image *iconImage(Package *pkg);
	Image *bannerImage(Package *pkg);
};
