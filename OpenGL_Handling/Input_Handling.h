#ifndef JAGUAR_INPUT_HANDLING_DECLARATIONS
#define JAGUAR_INPUT_HANDLING_DECLARATIONS

#include "OpenGL_Handling.h"

struct GLFWwindow;

namespace Jaguar
{
	struct Keystroke_Flags
	{
		GLuint Keycode;
		bool Pressed;	// This flag lets us know whether or not the key is down
		bool Changed;	// This flag lets us know if the 'pressed' value has changed since last frame

		// Important to keep track of 'fresh' presses of various keys
		// i.e. if you want a key to toggle something
	};

	struct Input_Data
	{
		std::vector<Keystroke_Flags> Keys;
	};

	void Get_User_Inputs(GLFWwindow* Window, Input_Data* Target_Input);
}

#endif