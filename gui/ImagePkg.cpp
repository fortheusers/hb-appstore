#include "ImagePkg.hpp"

namespace ImagePkg
{
	static const std::string defaultIconPath = ROMFS "res/default.png";

	static std::string iconUrl(Package *pkg)
	{
		return *(pkg->repoUrl) + "/packages/" + pkg->pkg_name + "/icon.png";
	}

	static std::string bannerUrl(Package *pkg)
	{
		return *(pkg->repoUrl) + "/packages/" + pkg->pkg_name + "/screen.png";
	}

	Image *iconImage(Package *pkg)
	{
		return new ImageNet(iconUrl(pkg), []{ return new ImageFile(defaultIconPath); });
	}

	Image *bannerImage(Package *pkg)
	{
#if defined(__WIIU__)
		// Use an icon banner
		ImageNet* banner = new ImageNet(iconUrl(pkg), []{
			// if the icon fails to load, use the default icon
			ImageFile *defaultIcon = new ImageFile(defaultIconPath);
			defaultIcon->setScaleMode(IMAGE_SCALE_BG);
			return defaultIcon;
		});
		banner->setScaleMode(IMAGE_SCALE_BG);
#else
		ImageNet *banner = new ImageNet(bannerUrl(pkg), [pkg]{
			// If the banner fails to load, use an icon banner
			ImageNet* icon = new ImageNet(iconUrl(pkg), []{
				// if even the icon fails to load, use the default icon
				ImageFile *defaultIcon = new ImageFile(defaultIconPath);
				defaultIcon->setScaleMode(IMAGE_SCALE_BG);
				return defaultIcon;
			});
			icon->setScaleMode(IMAGE_SCALE_BG);
			return icon;
		});
#endif
		return banner;
	}
}
