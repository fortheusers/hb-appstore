#include "InputEvents.hpp"

int TOTAL_BUTTONS = 13;

// computer key mappings
SDL_Keycode key_buttons[] = { SDLK_a, SDLK_b, SDLK_x, SDLK_y, SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_RETURN, SDLK_l, SDLK_r, SDLK_z, SDLK_BACKSPACE};

//SDL_GameControllerButton pad_buttons[] = { 0, 1, 2, 3, 13, 15, 12, 14, 10, 6, 7, 8, 11 };
SDL_GameControllerButton pad_buttons[] = { SDL_A, SDL_B, SDL_X, SDL_Y, SDL_UP, SDL_DOWN, SDL_LEFT, SDL_RIGHT, SDL_PLUS, SDL_L, SDL_R, SDL_ZL, SDL_MINUS };

// our own "buttons" that correspond to the above SDL ones
unsigned int ie_buttons[] = { A_BUTTON, B_BUTTON, X_BUTTON, Y_BUTTON, UP_BUTTON, DOWN_BUTTON, LEFT_BUTTON, RIGHT_BUTTON, START_BUTTON, L_BUTTON, R_BUTTON, Z_BUTTON, SELECT_BUTTON };

bool InputEvents::update()
{
  // get an event from SDL
  SDL_Event event;
  int ret = SDL_PollEvent(&event);
    
  // update our variables
  this->type = event.type;
  this->keyCode = -1;
  this->noop = false;
    
#ifdef PC
    this->allowTouch = false;
#endif

  if (this->type == SDL_KEYDOWN || this->type == SDL_KEYUP)
  {
    this->keyCode = event.key.keysym.sym;
  }
  else if (this->type == SDL_JOYBUTTONDOWN || this->type == SDL_JOYBUTTONUP)
  {
    this->keyCode = event.jbutton.button;
  }
    else if (this->type == SDL_JOYAXISMOTION)
    {
        // x values for left and right sticks
        if (event.jaxis.axis == 0 || event.jaxis.axis == 2) this->keyCode = (event.jaxis.value / 32767.0f < 0)? SDL_LEFT : SDL_RIGHT;
        
        // y values
        if (event.jaxis.axis == 1 || event.jaxis.axis == 3) this->keyCode = (event.jaxis.value / 32767.0f < 0)? SDL_UP : SDL_DOWN;
        
        // just simulate Dpad direction from stick input
        this->type = SDL_KEYDOWN;
    }
  else if (this->type == SDL_MOUSEMOTION || this->type == SDL_MOUSEBUTTONUP || this->type == SDL_MOUSEBUTTONDOWN)
  {
    bool isMotion = this->type == SDL_MOUSEMOTION;

    this->yPos = isMotion? event.motion.y : event.button.y;
    this->xPos = isMotion? event.motion.x : event.button.x;
  }
  else if (allowTouch && (this->type == SDL_FINGERMOTION || this->type == SDL_FINGERUP || this->type == SDL_FINGERDOWN))
  {
     this->yPos = event.tfinger.y * 720;
     this->xPos = event.tfinger.x * 1280;
  }
    
    // no more events to process
    if (ret == 0)
    {
        this->noop = true;
        return false;
    }

    return true;
}

bool InputEvents::held(int buttons)
{
  // if it's a key event
  if (this->type == SDL_KEYDOWN || this->type == SDL_KEYUP)
  {
    for (int x=0; x<TOTAL_BUTTONS; x++)
      if (key_buttons[x] == keyCode && (buttons & ie_buttons[x]))
        return true;
  }

  // if it's a controller event
  else if (this->type == SDL_JOYBUTTONDOWN || this->type == SDL_JOYBUTTONUP)
  {
    for (int x=0; x<TOTAL_BUTTONS; x++)
      if (pad_buttons[x] == keyCode && (buttons & ie_buttons[x]))
        return true;
  }

  return false;
}

//bool InputEvents::held(int buttons)
//{
//    // held is the same as held internal, except it works with a timer
//    // will return true for 1. a press down event or 2. 128ms after that
//    // and then
//}

bool InputEvents::pressed(int buttons)
{
    return isKeyDown() && held(buttons);
}

bool InputEvents::released(int buttons)
{
    return isKeyUp() && held(buttons);
}

bool InputEvents::touchIn(int x, int y, int width, int height)
{
  return (this->xPos >= x &&
          this->xPos <= x+width &&
          this->yPos >= y &&
          this->yPos <= y+height);
}

bool InputEvents::isTouchDown()
{
    return this->type == SDL_MOUSEBUTTONDOWN || (allowTouch && this->type == SDL_FINGERDOWN);
}

bool InputEvents::isTouchDrag()
{
    return this->type == SDL_MOUSEMOTION || (allowTouch && this->type == SDL_FINGERMOTION);
}

bool InputEvents::isTouchUp()
{
    return this->type == SDL_MOUSEBUTTONUP || (allowTouch && this->type == SDL_FINGERUP);
}

bool InputEvents::isTouch()
{
    return isTouchDown() || isTouchDrag() || isTouchUp();
}

bool InputEvents::isKeyDown()
{
  return this->type == SDL_KEYDOWN || this->type == SDL_JOYBUTTONDOWN;
}

bool InputEvents::isKeyUp()
{
  return this->type == SDL_KEYUP || this->type == SDL_JOYBUTTONUP;
}
