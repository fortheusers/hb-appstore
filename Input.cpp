#include "Input.hpp"

Input::Input()
{
	#if defined(LINUX) || defined(__APPLE__)
	#else // switch
		hid_init();
	#endif
}

void Input::close()
{
	#if defined(LINUX) || defined(__APPLE__)
	#else // switch
		hid_finalize();
	#endif
}

#if defined(LINUX) || defined(__APPLE__)
#else // switch
void Input::readInputInternal(hid_controller_state_entry_t ent)
{
	// process inputs
	this->btns_h |= ((ent.button_state & JOYPAD_A)? 		BUTTON_A : 0);
	this->btns_h |= ((ent.button_state & JOYPAD_B)? 		BUTTON_B : 0);
	this->btns_h |= ((ent.button_state & JOYPAD_UP)? 		BUTTON_UP : 0);
	this->btns_h |= ((ent.button_state & JOYPAD_DOWN)? 		BUTTON_DOWN : 0);
	this->btns_h |= ((ent.button_state & JOYPAD_LEFT)? 		BUTTON_LEFT : 0);
	this->btns_h |= ((ent.button_state & JOYPAD_RIGHT)?		BUTTON_RIGHT : 0);
	this->btns_h |= ((ent.button_state & JOYPAD_START)?		BUTTON_PLUS : 0);
	this->btns_h |= ((ent.button_state & JOYPAD_SELECT)?	BUTTON_MINUS : 0);

	// get int values of sticks
	int p1 = ent.left_stick_x, p2 = ent.left_stick_y, p3 = ent.right_stick_x, p4 = ent.right_stick_y;

	// make doubles
	double d1 = (float) p1, d2 = (float) p2, d3 = (float) p3, d4 = (float) p4;

	// update stick values (between -1 and 1 for each)
	this->lstick_x += d1 / 32768.0;
	this->lstick_y += d2 / 32768.0;
	this->rstick_x += d3 / 32768.0;
	this->rstick_y += d4 / 32768.0;
}
#endif

void Input::updateButtons()
{
	// reset buttons
	this->btns_h = 0b00000000;
	
	#if defined(LINUX) || defined(__APPLE__)
		SDL_Event event;
		SDL_PollEvent(&event);
	
		this->lstick_x = 0;
		this->lstick_y = 0;
		this->rstick_x = 0;
		this->rstick_x = 0;
		
		if (event.type == SDL_KEYDOWN)
		{
			printf("got into doing a keydown\n");
			this->btns_h |= ((event.key.keysym.sym == SDLK_RETURN)?		BUTTON_A : 0);
			this->btns_h |= ((event.key.keysym.sym == SDLK_BACKSPACE)?		BUTTON_B : 0);
			this->btns_h |= ((event.key.keysym.sym == SDLK_UP)?		BUTTON_UP : 0);
			this->btns_h |= ((event.key.keysym.sym == SDLK_DOWN)?		BUTTON_DOWN : 0);
			this->btns_h |= ((event.key.keysym.sym == SDLK_LEFT)?		BUTTON_LEFT : 0);
			this->btns_h |= ((event.key.keysym.sym == SDLK_RIGHT)?		BUTTON_RIGHT : 0);
			this->btns_h |= ((event.key.keysym.sym == SDLK_SPACE)?		BUTTON_PLUS : 0);
			this->btns_h |= ((event.key.keysym.sym == SDLK_DELETE)?		BUTTON_MINUS : 0);
			
			// update stick values (between -1 and 1 for each)
			this->rstick_x = -1*(event.key.keysym.sym == SDLK_w) + (event.key.keysym.sym == SDLK_s);
			this->rstick_y = -1*(event.key.keysym.sym == SDLK_a) + (event.key.keysym.sym == SDLK_d);
		}
	
	#else // switch
		this->lstick_x = 0;
		this->lstick_y = 0;
		this->rstick_x = 0;
		this->rstick_y = 0;

		// scan for controllers (main unit or 1P joycons detached)
		hid_controller_t* num = hid_get_shared_memory()->controllers;		// joycons detached
		hid_controller_t* num8 = hid_get_shared_memory()->controllers + 8;	// main unit

		hid_controller_state_entry_t ent = num->main.entries[num->main.latest_idx];
		hid_controller_state_entry_t ent8 = num8->main.entries[num8->main.latest_idx];

		// process inputs
		this->readInputInternal(ent);
		this->readInputInternal(ent8);

	#endif
}

bool Input::held(char b)
{
	return this->btns_h & b;
}