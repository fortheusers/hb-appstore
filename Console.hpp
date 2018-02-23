#include <SDL2/SDL.h>

class Console
{
public:
	// console methods
	Console();
	void background(int r, int g, int b);
	void putAPixel(int x, int y, int r, int g, int b);
	void drawString(int xi, int yi, const char* string);
	void drawColorString(int xi, int yi, const char* string, int r, int g, int b);
	void close();
	
private:
	void update();
	
	// SDL graphics variables
	SDL_Window* window;
	SDL_Surface* window_surface;
};

void sleep(int s);
char* fontLookup(char c);