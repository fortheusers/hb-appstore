#include "../libs/chesto/src/DrawUtils.hpp"

class Console
{
public:
	// console methods
	Console(CST_Window* window);
	void background(int r, int g, int b);
	void putAPixel(int x, int y, int r, int g, int b);
	void fillRect(int x, int y, int width, int height, int r, int g, int b);
	void drawString(int xi, int yi, const char* string);
	void drawColorString(int xi, int yi, const char* string, int r, int g, int b);
	void close();
	void update();

private:
	// SDL graphics variables
	CST_Surface* window_surface;
	CST_Window* window;
};

void sleep(int s);
char* fontLookup(char c);
