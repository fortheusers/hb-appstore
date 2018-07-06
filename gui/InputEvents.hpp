#include <SDL2/SDL.h>

#define LEFT_BUTTON		0b00000001
#define RIGHT_BUTTON	0b00000010
#define UP_BUTTON	  	0b00000100
#define DOWN_BUTTON		0b00001000

#define START_BUTTON	0b00010000
// #define BUTTON_MINUS	0b00100000

#define A_BUTTON	   	0b01000000
#define B_BUTTON		  0b00100000

class InputEvents
{
public:
  // whether or not a button is pressed during this cycle
  bool held(int buttons);

  // whether or not a touch is detected within the specified rect in this cycle
  bool touchIn(int x, int y, int width, int height);

  // wait for a cycle (currently just hardcoded to 16ms, should dynamically adapt using the clock)
  void delay();

  // update which buttons are pressed
  void update();

  // whether or not the current event is one of a few known ones
  bool isTouchDown();
  bool isTouchUp();
  bool isTouchDrag();
  bool isKeyDown();
  bool isKeyUp();

  SDL_Keycode keyCode = NULL;
  // SDL_Con

  void* actionable = NULL;

  int yPos = 0, xPos = 0;

private:
  Uint32 type;
};
