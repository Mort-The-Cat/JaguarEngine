#include "Inputs.hpp"
#include "../JaguarEngine.hpp"

namespace Jaguar
{
	template<int (*Function)(GLFWwindow*, int)>
	void Get_Buttons(JaguarEngine* Engine, Button_Flags* Buttons, size_t Count)
	{
		for (size_t Index = 0; Index < Count; Index++)
		{
			bool New = GLFW_PRESS == Function(Engine->Window_Info.Window, Buttons[Index].Keycode);
			Buttons[Index].Changed = Buttons[Index].Pressed != New;	// If they're different, it's changed.
			Buttons[Index].Pressed = New;
		}
	}

	void Get_User_Inputs(JaguarEngine* Engine)
	{
		Get_Buttons<glfwGetKey>(Engine, Engine->Inputs.Keys.data(), Engine->Inputs.Keys.size());

		// If using controller, mouse inputs will be handled by player 1's right joystick probably

		double X, Y;
		glfwGetCursorPos(Engine->Window_Info.Window, &X, &Y);

		if (Engine->Inputs.Mouse.Reset_Mouse_Cursor)
		{
			glfwSetCursorPos(Engine->Window_Info.Window, Engine->Window_Info.Height * 0.5f, Engine->Window_Info.Height * 0.5f);
			Engine->Inputs.Mouse.Previous = glm::vec2(0.5f, 0.5f);
		}
		else
			Engine->Inputs.Mouse.Previous = Engine->Inputs.Mouse.Cursor;

		Engine->Inputs.Mouse.Cursor.x = X / (double)Engine->Window_Info.Height;
		Engine->Inputs.Mouse.Cursor.y = Y / (double)Engine->Window_Info.Height;

		// mouse buttons

		Get_Buttons<glfwGetMouseButton>(Engine, Engine->Inputs.Mouse.Buttons, 3);
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
			Engine->Inputs.Text_Input.push_back(-1);	// Adds bullshit character that needs to be processed
			return;											// NOTE: It is the UI's job to handle backspacing.

		case GLFW_KEY_TAB:
			Engine->Inputs.Text_Input.push_back(U'\t');		// 'tab' character
			return;
		}
	}

	void Callback_Read_Text_Input(GLFWwindow* Window, unsigned int Key)	// utf32
	{
		JaguarEngine* Engine = (JaguarEngine*)glfwGetWindowUserPointer(Window);	// Necessary to get Engine from callback function

		if (Key == GLFW_KEY_BACKSPACE)						// If this is a backspace key?
			Engine->Inputs.Text_Input.resize(
				Engine->Inputs.Text_Input.size() - 
				(Engine->Inputs.Text_Input.size() != 0)		// Reduce by 1 char if possible
			);
		else												// Otherwise? 
			Engine->Inputs.Text_Input.push_back(Key);		// append this key to the user's text input string

		//printf("%d\n", Key);
	}
}