#include "Input.hpp"

Input::Input()
{
}

void Input::close()
{
}

void Input::updateButtons()
{
	// reset buttons
	this->btns_h = 0b00000000;
	
	SDL_Event event;
	SDL_PollEvent(&event);

	this->lstick_x = 0;
	this->lstick_y = 0;
	this->rstick_x = 0;
	this->rstick_x = 0;
	
	if (event.type == SDL_KEYDOWN)
	{
		this->btns_h |= ((event.key.keysym.sym == SDLK_a)?			BUTTON_A : 0);
		this->btns_h |= ((event.key.keysym.sym == SDLK_b)?			BUTTON_B : 0);
		this->btns_h |= ((event.key.keysym.sym == SDLK_UP)?			BUTTON_UP : 0);
		this->btns_h |= ((event.key.keysym.sym == SDLK_DOWN)?		BUTTON_DOWN : 0);
		this->btns_h |= ((event.key.keysym.sym == SDLK_LEFT)?		BUTTON_LEFT : 0);
		this->btns_h |= ((event.key.keysym.sym == SDLK_RIGHT)?		BUTTON_RIGHT : 0);
		this->btns_h |= ((event.key.keysym.sym == SDLK_MINUS|| event.key.keysym.sym == SDLK_RETURN)?		BUTTON_MINUS : 0);
		this->btns_h |= ((event.key.keysym.sym == SDLK_x)?			BUTTON_X : 0);
	}
}

bool Input::held(char b)
{
	return this->btns_h & b;
}
