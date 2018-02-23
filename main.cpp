#include "Console.hpp"

int main()
{
	// initialize text console
	Console* console = new Console();

	// draw hello world string
	console->drawString(2, 1, "Hello world!");

	// wait 10s
	sleep(10);
	console->close();

	return 0;
}
