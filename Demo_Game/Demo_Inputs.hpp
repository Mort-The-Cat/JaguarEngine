#ifndef DEMO_INPUTS
#define DEMO_INPUTS

#include "../JaguarEngine/JaguarEngine.hpp"

namespace Demo_Controls
{
	enum
	{
		Forwards = 0,
		Left,
		Backwards,
		Right,

		Up,
		Down,

		Escape,

		Number_Of_Keys
	};

	// It bothers me deeply that these are separate declarations but I'm not sure of a better way

	const GLuint Keycodes[Number_Of_Keys] = 
	{
		GLFW_KEY_W,
		GLFW_KEY_A,
		GLFW_KEY_S,
		GLFW_KEY_D,

		GLFW_KEY_SPACE,
		GLFW_KEY_LEFT_SHIFT,

		GLFW_KEY_ESCAPE
	};
}

void Demo_Init_Inputs(Jaguar::JaguarEngine* Engine)
{
	Engine->Inputs.Keys.resize(Demo_Controls::Number_Of_Keys);

	for (int Index = 0; Index < Demo_Controls::Number_Of_Keys; Index++)
	{
		Engine->Inputs.Keys[Index].Keycode = Demo_Controls::Keycodes[Index];
	}
}

#endif