#include <SDL2/SDL.h>

// clang-format off
#define LEFT_BUTTON       0b00000000000001
#define RIGHT_BUTTON      0b00000000000010
#define UP_BUTTON         0b00000000000100
#define DOWN_BUTTON       0b00000000001000
#define START_BUTTON      0b00000000010000
#define B_BUTTON          0b00000000100000
#define A_BUTTON          0b00000001000000
#define ZL_BUTTON         0b00000010000000
#define SELECT_BUTTON     0b00000100000000
#define L_BUTTON          0b00001000000000
#define R_BUTTON          0b00010000000000
#define X_BUTTON          0b00100000000000
#define Y_BUTTON          0b01000000000000
#define ZR_BUTTON         0b10000000000000

// SDL enums should line up with the actual controls
// uses switch+wiiu mappings, see: https://github.com/rw-r-r-0644/sdl2-wiiu/blob/master/SDL2-wiiu/src/joystick/wiiu/SDL_sysjoystick.c#L38
#define SDL_A        SDL_CONTROLLER_BUTTON_A
#define SDL_B        SDL_CONTROLLER_BUTTON_B
#define SDL_X        SDL_CONTROLLER_BUTTON_X
#define SDL_Y        SDL_CONTROLLER_BUTTON_Y

#define SDL_PLUS     SDL_CONTROLLER_BUTTON_RIGHTSHOULDER
#define SDL_L        SDL_CONTROLLER_BUTTON_START
#define SDL_R        SDL_CONTROLLER_BUTTON_LEFTSTICK
#define SDL_ZL       SDL_CONTROLLER_BUTTON_RIGHTSTICK
#define SDL_ZR       SDL_CONTROLLER_BUTTON_LEFTSHOULDER
#define SDL_MINUS    SDL_CONTROLLER_BUTTON_DPAD_UP

#define SDL_UP        SDL_CONTROLLER_BUTTON_DPAD_LEFT
#define SDL_DOWN      SDL_CONTROLLER_BUTTON_MAX
#define SDL_LEFT      SDL_CONTROLLER_BUTTON_DPAD_DOWN
#define SDL_RIGHT     SDL_CONTROLLER_BUTTON_DPAD_RIGHT

#define SDL_LEFT_STICK   (SDL_GameControllerButton)16
#define SDL_UP_STICK     (SDL_GameControllerButton)17
#define SDL_RIGHT_STICK  (SDL_GameControllerButton)18
#define SDL_DOWN_STICK   (SDL_GameControllerButton)19
// clang-format on

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

	// additional key processing info
	bool processDirectionalButtons();
	int directionForKeycode();
	void toggleHeldButtons();

	// joystick device events processing
	void processJoystickHotplugging(SDL_Event *event);

	SDL_Keycode keyCode = -1;

	bool held_directions[4] = { false, false, false, false };
	int rapidFireRate = 12; // fire duplicate events if curframe mod rapidFireRate is 0 (higher = slower)
	int curFrame = 0;

#ifdef PC
	int wheelScroll = 0;
#endif

	int yPos = 0;
	int xPos = 0;
	bool noop = false;

private:
	Uint32 type;
};
