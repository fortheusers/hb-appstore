#include "../libs/get/src/Get.hpp"
#include "../libs/get/src/Utils.hpp"

#include "../libs/chesto/src/InputEvents.hpp"
#include "../libs/chesto/src/RootDisplay.hpp"

#include "../gui/main.hpp"

#include "Menu.hpp"

int console_main(RootDisplay* rootDisplay, InputEvents* input)
{
	// initialize text console
  SDL_DestroyRenderer(rootDisplay->renderer);
  rootDisplay->renderer = NULL;
	Console* console = new Console(rootDisplay->window);

	// create main menu object
	Menu* menu = new Menu(console);

	bool running = true;

	while (running)
	{
		console->background(42, 37, 39);

		// show the current menu screen
		menu->display();
		SDL_Delay(4);

		// update pressed buttons in input object
		input->update();

		// if we're on the install screen, perform an install
		if (menu->screen == INSTALLING || menu->screen == REMOVING)
		{
			Package* target = menu->get->packages[menu->position];

			// install package
			bool succeeded = false;

			if (menu->screen == INSTALLING)
				succeeded = menu->get->install(target);
			else if (menu->screen == REMOVING)
				succeeded = menu->get->remove(target);

			// change screen accordingly
			if (succeeded)
				menu->screen = INSTALL_SUCCESS;
			else
				menu->screen = INSTALL_FAILED;
		}

    if (menu->screen == RECOVERY_OPTIONS)
    {
      // three bools to represent the L+R+A activation sequence
      bool pressed[3] = { false, false, false };
      int triggers[3] = { L_BUTTON, R_BUTTON, A_BUTTON };

      while (true)
      {
        input->update();

        if (input->pressed(B_BUTTON))
        {
          menu->screen = SPLASH;
          break;
        }
        else if (input->held(L_BUTTON | R_BUTTON | A_BUTTON))
        {
          int count = 0;
          for (int x=0; x<3; x++)
          {
            pressed[x] |= input->held(triggers[x]);
            count += pressed[x];
          }

          if (count == 3)
          {
            // reset data, and switch to recovery success screen
            bool success = libget_reset_data(DEFAULT_GET_HOME);
            menu->get = NULL;
            menu->screen = success ? INSTALL_SUCCESS : INSTALL_FAILED;
            break;
          }
        }
        if (input->released(L_BUTTON | R_BUTTON | A_BUTTON))
          // reset the pressed array since one of the three buttons was released
          for (int x=0; x<3; x++)
            pressed[x] = false;
      }

      continue;
    }

		// send either A or B to the menu object, if held
		if (input->pressed(A_BUTTON) || input->pressed(B_BUTTON))
			menu->advanceScreen(input->pressed(A_BUTTON));

		if (menu->screen == INSTALL_SCREEN && input->pressed(X_BUTTON))
			menu->screen = REMOVING;

		// if minus is pressed, exit
		if (input->pressed(SELECT_BUTTON) || input->pressed(START_BUTTON))
			running = false;

		// if X is pressed on the splash screen, show advanced options
    if (menu->screen == SPLASH && input->pressed(Y_BUTTON))
      menu->screen = RECOVERY_OPTIONS;

		// move cursor up or down depending on input
		menu->moveCursor(-1 * (input->pressed(UP_BUTTON)) + (input->pressed(DOWN_BUTTON)));

		// move page PAGE_SIZE forward/backward depending on input
		menu->moveCursor(-1 * PAGE_SIZE * input->pressed(LEFT_BUTTON) + PAGE_SIZE * input->pressed(RIGHT_BUTTON));
	}

	console->close();

	return 0;
}
