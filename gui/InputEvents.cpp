#include "InputEvents.hpp"

int TOTAL_BUTTONS = 7;
SDL_Keycode key_buttons[] = { SDLK_a, SDLK_b, SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_RETURN };
SDL_GameControllerButton pad_buttons[] = { SDL_CONTROLLER_BUTTON_A, SDL_CONTROLLER_BUTTON_B, SDL_CONTROLLER_BUTTON_DPAD_UP, SDL_CONTROLLER_BUTTON_DPAD_DOWN, SDL_CONTROLLER_DPAD_LEFT, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, SDL_CONTROLLER_BUTTON_START };
char ie_buttons[] = { A_BUTTON, B_BUTTON, UP_BUTTON, DOWN_BUTTON, LEFT_BUTTON, RIGHT_BUTTON, START_BUTTON };

void InputEvents::update()
{
  // get an event from SDL
  SDL_Event event;
  SDL_PollEvent(&event);

  // update our variables
  this->type = event.type;

  this->keyCode = NULL;
  this->padCode = NULL;

  if (this->type == SDL_KEYDOWN || this->type == SDL_KEYUP)
  {
    this->keyCode = event.key.keysym.sym;
  }
  else if (this->type == SDL_JOYBUTTONDOWN || this->type == SDL_JOYBUTTONUP)
  {
    this->padCode = event.jbutton.which;
  }
  else if (this->type == SDL_MOUSEMOTION || this->type == SDL_MOUSEBUTTONUP || this->type == SDL_MOUSEBUTTONDOWN)
  {
    this->yPos = event.motion.y;
    this->xPos = event.motion.x;
  }
  else if (this->type == SDL_FINGERMOTION || this->type == SDL_FINGERUP || this->type == SDL_FINGERDOWN)
  {
    // this->yPos = event.motion.y;
    // this->xPos = event.motion.x;
  }
  else
    this->actionable = NULL;
}

bool InputEvents::held(int buttons)
{
  // if it's a key down event
  if (this->actionable != NULL)
  {
    for (int x=0; x<TOTAL_BUTTONS; x++)
      if (this->actionable[x] == keyCode && (buttons & ie_buttons[x]))
        return true;
  }

  return false;
}

bool InputEvents::isTouchDown()
{
  return this->type == SDL_MOUSEBUTTONDOWN || this->type == SDL_FINGERDOWN;
}

bool InputEvents::isTouchDrag()
{
  return this->type == SDL_MOUSEMOTION || this->type == SDL_FINGERMOTION;
}

bool InputEvents::isTouchUp()
{
  return this->type == SDL_MOUSEBUTTONUP || this->type == SDL_FINGERUP;
}

bool InputEvents::isKeyDown()
{
  return this->type == SDL_KEYDOWN || this->type == SDL_JOYBUTTONDOWN;
}

bool InputEvents::isKeyUp()
{
  return this->type == SDL_KEYUP || this->type == SDL_JOYBUTTONUP;
}

void InputEvents::delay()
{
	SDL_Delay(16);
}
