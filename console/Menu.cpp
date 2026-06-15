#include "Menu.hpp"
#include "../gui/main.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include <algorithm>
#include <sstream>

using namespace std;
using namespace Chesto;

Menu::Menu(Console* console)
{
	this->console = console;
	this->position = 0;
	this->screen = SPLASH;
}

bool Menu::useLargeFont() const
{
	// use large font (unifont) for all languages except en-us
	return (TextElement::curLang != "en-us");
}

int Menu::getPageSize() const
{
	return useLargeFont() ? PAGE_SIZE_LARGE_FONT : PAGE_SIZE;
}

void Menu::display()
{
	bool largeFnt = useLargeFont();
	int topBarHeight = largeFnt ? 2 : 1;
	int bottomBarY = largeFnt ? 43 : 44;
	int bottomBarHeight = largeFnt ? 2 : 1;

	if (this->screen == SPLASH)
	{
		console->drawString(9, 21, i18n("recovery.title"));
		console->drawColorString(9, 26, i18n("recovery.thanks"), 0xcc, 0xcc, 0xcc);
		console->drawColorString(15, 27 + int(largeFnt), "vgmoose, pwsincd, rw-r-r_0644, zarklord", 0xcc, 0xcc, 0xcc);
		// TODO: credit specific language translator for current locale
		console->drawColorString(9, 32, i18n("recovery.continue"), 0xff, 0xff, 0x00);
    	console->drawColorString(9, 34, i18n("recovery.reset"), 0xff, 0xff, 0x00);
	}

  if (this->screen == RECOVERY_OPTIONS)
  {
		
		console->fillRect(0, 0, 80, topBarHeight, 0, 0, 255);
		auto titleText = i18n("recovery.reset.title");
		int centeredX = largeFnt ? 28 : 80 / 2 - 12;
		console->drawColorString(centeredX, 0, titleText, 0xff, 0xff, 0xff);

		console->drawString(9, 16, i18n("recovery.proverb1"));
		console->drawString(9, 18, i18n("recovery.proverb2"));

		console->drawColorString(9, 23, i18n("recovery.instructions"), 0xff, 0xff, 0x00);

		console->drawString(9, 28, i18n("recovery.report"));

		console->fillRect(0, bottomBarY, 80, bottomBarHeight, 0, 0, 255);
		console->drawColorString(0, bottomBarY, i18n("recovery.back"), 0xff, 0xff, 0xff);
  }

	if (this->get == NULL && this->screen != SPLASH && this->screen != RECOVERY_OPTIONS
      && this->screen != INSTALL_SUCCESS && this->screen != INSTALL_FAILED)
	{
		// if libget isn't initialized, and we're trying to load a get-related screen, init it!
		console->update();
		console->drawString(3, 11, i18n("recovery.syncing"));
		console->drawColorString(3, 13, i18n("recovery.plswait"), 0, 0xcc, 0xcc);
		console->update();
		initGet();
		return;
	}

	if (this->screen == LIST_MENU || this->screen == INSTALL_SCREEN)
	{
		// draw the top bar
		console->fillRect(0, 0, 80, topBarHeight, 255, 255, 0);
		auto titleText = i18n("recovery.title2");
		int centeredX = largeFnt ? 28 : 80 / 2 - 15;
		console->drawColorString(centeredX, 0, titleText, 0, 0, 0);

		// draw bottom bar
		console->fillRect(0, bottomBarY, 80, bottomBarHeight, 255, 255, 0);
		console->drawColorString(0, bottomBarY, i18n("recovery.options"), 0, 0, 0);
		console->drawColorString(80 - (this->repoUrl.length() + 2), bottomBarY, this->repoUrl, 0, 0, 0);
	}

	if (this->screen == LIST_MENU)
	{
		int pageSize = getPageSize();
		int lineSpacing = largeFnt ? 4 : 3;
		int startY = largeFnt ? 3 : 3;
		
		int start = (this->position / pageSize) * pageSize;

		// go through this page of apps until the end of the page, or longer than the packages list
		size_t packageCount = get->getPackages().size();
		for (int x = start; x < start + pageSize && (size_t)x < packageCount; x++)
		{
			int curPosition = (x % pageSize) * lineSpacing + startY;

			auto cur = get->list()[x];
			std::stringstream line;
			line << cur.getTitle() << " (" << cur.getVersion() << ")";
			console->drawString(15, curPosition, line.str());

			auto status = cur.getStatus();

			int r = (status == UPDATE || status == LOCAL) ? 0xFF : 0x00;
			int g = (status == UPDATE) ? 0xF7 : 0xFF;
			int b = (status == INSTALLED || status == LOCAL) ? 0xFF : 0x00;
			console->drawColorString(5, curPosition, cur.statusString(), r, g, b);

			std::stringstream line2;
			line2 << cur.getShortDescription() << " [" << cur.getAuthor() << "]";

			console->drawColorString(16, curPosition + 1 + int(largeFnt), line2.str(), 0xcc, 0xcc, 0xcc);
		}

		std::stringstream footer;
		footer << i18n("recovery.footer1") << this->position / pageSize + 1 << i18n("recovery.footer2") << (get->getPackages().size()-1) / pageSize + 1;
		int footerY = largeFnt ? 37 : 40;
		console->drawString(34, footerY, footer.str());
		auto footerText = i18n("recovery.pages");

		int centeredX = largeFnt ? 7 : 15;
		console->drawColorString(centeredX, footerY + 2, footerText, 0xcc, 0xcc, 0xcc);

		console->drawString(1, (this->position % pageSize) * lineSpacing + startY + int(largeFnt), largeFnt ? " >>" : "-->");
	}

	if (this->screen == INSTALL_SCREEN)
	{
		size_t packageCount = get->getPackages().size();
		if (this->position < 0 || (size_t)this->position >= packageCount)
		{
			// invalid selection, go back a screen
			this->screen--;
			return;
		}

		// currently selected package
		auto cur = get->list()[this->position];

		console->drawString(5, 3, cur.getTitle());
		console->drawString(6, 5, cur.getVersion());
		int gap = largeFnt ? 1 : 0;
		console->drawString(6, 6 + gap, cur.getAuthor());

		auto status = cur.getStatus();

		int r = (status == UPDATE || status == LOCAL) ? 0xFF : 0x00;
		int g = (status == UPDATE) ? 0xF7 : 0xFF;
		int b = (status == INSTALLED || status == LOCAL) ? 0xFF : 0x00;
		console->drawColorString(5, 8 + gap, cur.statusString(), r, g, b);

		console->drawColorString(5, 12, i18n("recovery.actions.install"), 0xff, 0xff, 0x00);

		int offset = 0;
		if (status != GET && status != LOCAL)
			console->drawColorString(5, 14, i18n("recovery.actions.remove"), 0xff, 0xff, 0x00);
		else
			offset = 2;

		console->drawString(5, 16 - offset, i18n("recovery.actions.back"));
	}

	if (this->screen == INSTALLING || this->screen == REMOVING)
	{
		// currently selected package
		auto cur = get->list()[this->position];

		console->drawString(5, 4, cur.getTitle());

		int gap = largeFnt ? 1 : 0;
		if (this->screen == INSTALLING)
			console->drawColorString(5, 5 + gap, i18n("recovery.downloading"), 0xff, 0xff, 0x00);
		else
			console->drawColorString(5, 5 + gap, i18n("recovery.removing"), 0xff, 0xff, 0x00);

    	console->drawString(5, 9, i18n("recovery.warning1"));
		console->drawColorString(5, 11, i18n("recovery.plswait2"), 0, 0xcc, 0xcc);

		console->drawString(5, 15, i18n("recovery.report"));

	}

	if (this->screen == INSTALL_SUCCESS || this->screen == INSTALL_FAILED)
	{
		if (this->screen == INSTALL_SUCCESS)
			console->drawColorString(3, 12, i18n("recovery.success"), 0, 0xff, 0);
		else
		{
			console->drawColorString(3, 10, i18n("recovery.failure"), 0xff, 0, 0);
			console->drawString(3, 12, i18n("recovery.cta"));
		}

		console->drawColorString(3, 14, i18n("recovery.pressa"), 0xff, 0xff, 0x00);
	}

	console->update();
}

void Menu::initGet()
{
	// this is a blocking load
#if defined(WII) || defined(WII_MOCK)
	// default the repo type to OSC for wii (TODO: don't hardcode this)
	this->get = new Get(DEFAULT_GET_HOME, DEFAULT_REPO, true, "osc");
#elif defined(_3DS) || defined(_3DS_MOCK)
	// default the repo type to universaldb for 3ds (TODO: don't hardcode this)
	this->get = new Get(DEFAULT_GET_HOME, DEFAULT_REPO, true, "unistore");
#else
	this->get = new Get(DEFAULT_GET_HOME, DEFAULT_REPO);
#endif

	// recovery GUI does not check the metarepo for updates

	if (get->getRepos().size() > 0)
		this->repoUrl = get->getRepos()[0]->getUrl();
	else
		this->repoUrl = i18n("recovery.nopkgs");
}

void Menu::moveCursor(int diff)
{
	if (this->get == NULL) return;

	this->position += diff;

	size_t packageCount = get->getPackages().size();
	if (position < 0)
	{
		// went back too far, wrap around to last package
		position = (int)packageCount - 1;
	}

	else if ((size_t)position >= packageCount)
	{
		// too far forward, wrap around to first package
		position = 0;
	}
}

void Menu::advanceScreen(bool advance)
{
	if (advance)
	{
		// A on these screens just returns to app list
		if (this->screen == INSTALL_SUCCESS || this->screen == INSTALL_FAILED)
		{
			this->screen = LIST_MENU;
		}
		// if on the install screen, or install-related screens, A does nothing
		else if (this->screen < INSTALLING)
		{
			// just advance the screen
			this->screen++;
		}
	}
	else
	{
		// go back, unless we can't anymore, or doing an install-related action
		if (this->screen > SPLASH && this->screen < INSTALLING)
			this->screen--;
	}
	printf("current screen: %d\n", this->screen);
}
