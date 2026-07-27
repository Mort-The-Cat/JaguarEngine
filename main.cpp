#include "Demo_Game/Demo.hpp"

int main()
{
	Jaguar::JaguarEngine Engine;

	if (Jaguar::Init_Jaguar(&Engine))
	{
		printf(" >> Failed to init JaguarEngine!\n\n");

		return -1;
	}

	if (Jaguar::Init_Window(&Engine, 1280, 960))
	{
		printf(" >> Failed to init window!\n\n");

		return -1;
	}

	Run_Scene(&Engine);

	Jaguar::End_Jaguar(&Engine);

	return 0;
}