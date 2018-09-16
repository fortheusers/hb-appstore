#include <SDL2/SDL.h>

#define LEFT_BUTTON		0b00000001
#define RIGHT_BUTTON	0b00000010
#define UP_BUTTON	  	0b00000100
#define DOWN_BUTTON		0b00001000
#define START_BUTTON	0b00010000
#define B_BUTTON		0b00100000
#define A_BUTTON        0b01000000
#define Z_BUTTON        0b10000000

// NOT IMPLEMENTED YET
#define L_BUTTON        0b00000000
#define R_BUTTON        0b00000000
#define SELECT_BUTTON   0b00000000




class InputEvents
{
public:
  // whether or not a button is pressed during this cycle
  bool held(int buttons);
  bool pressed(int buttons);
  bool released(int buttons);

  // whether or not a touch is detected within the specified rect in this cycle
  bool touchIn(int x, int width, int y, int height);

  // update which buttons are pressed
  bool update();
    
    bool allowTouch = true;

  // whether or not the current event is one of a few known ones
  bool isTouchDown();
  bool isTouchUp();
  bool isTouchDrag();
    bool isTouch();
  bool isKeyDown();
  bool isKeyUp();

  SDL_Keycode keyCode;

  int yPos = 0, xPos = 0;
    bool noop = false;

private:
  Uint32 type;
};
