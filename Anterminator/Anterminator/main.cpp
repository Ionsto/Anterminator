#include <iostream>
#include "GameManager.h"

int main(int argc, char** args)
{
	std::cout << "Starting game" << std::endl;
	GameManager gm;
	gm.Run();
	std::cout << "Closing" << std::endl;
}