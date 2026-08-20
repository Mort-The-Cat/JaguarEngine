#include "Inputs.hpp"
#include "../JaguarEngine.hpp"

namespace Jaguar
{
	void Get_User_Inputs(JaguarEngine* Engine)
	{
		for (size_t Index = 0; Index < Engine->Inputs.Keys.size(); Index++)
		{
			bool New = GLFW_PRESS == glfwGetKey(Engine->Window_Info.Window, Engine->Inputs.Keys[Index].Keycode);
			Engine->Inputs.Keys[Index].Changed = Engine->Inputs.Keys[Index].Pressed != New;	// If they're different, it's changed.
			Engine->Inputs.Keys[Index].Pressed = New;
		}


	}

	void Callback_Read_Special_Input(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods)
	{
		JaguarEngine* Engine = (JaguarEngine*)glfwGetWindowUserPointer(Window);

		if (Action != GLFW_PRESS && Action != GLFW_REPEAT)
			return;

		switch (Key)
		{
		case GLFW_KEY_ENTER:
			Engine->Inputs.Text_Input.push_back(U'\n');		// newline character
			return;

		case GLFW_KEY_BACKSPACE:
			//printf(" >> Backspace!\n");					// Test
			Engine->Inputs.Text_Input.push_back('\uFFFF');	// Adds bullshit character that needs to be processed
			return;											// NOTE: It is the UI's job to handle backspacing.

		case GLFW_KEY_TAB:
			Engine->Inputs.Text_Input.push_back(U'\t');		// 'tab' character
			return;
		}
	}

	void Callback_Read_Text_Input(GLFWwindow* Window, unsigned int Key)
	{
		JaguarEngine* Engine = (JaguarEngine*)glfwGetWindowUserPointer(Window);	// Necessary to get Engine from callback function

		if (Key == GLFW_KEY_BACKSPACE)						// If this is a backspace key?
			Engine->Inputs.Text_Input.resize(
				Engine->Inputs.Text_Input.size() - 
				(Engine->Inputs.Text_Input.size() != 0)		// Reduce by 1 char if possible
			);
		else												// Otherwise? 
			Engine->Inputs.Text_Input.push_back(Key);		// append this key to the user's text input string
	}
}