#include "JaguarEngine/JaguarEngine.hpp"

int main()
{
	Jaguar::JaguarEngine Engine;

	if (Jaguar::Init_Jaguar(&Engine))
	{
		printf(" >> Failed to init JaguarEngine!\n\n");

		return 1;
	}

	if (Jaguar::Init_Window(&Engine))
	{
		printf(" >> Failed to init window!\n\n");

		return 1;
	}

	while (!glfwWindowShouldClose(Engine.Window_Info.Window))
	{
		glfwPollEvents();
		glfwSwapBuffers(Engine.Window_Info.Window);
	}

	Jaguar::End_Jaguar(&Engine);

	return 0;
}