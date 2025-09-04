#include "Input_Handling.h"

namespace Jaguar
{

	void Get_User_Inputs(GLFWwindow* Window, Input_Data* Target_Input)
	{
		for (size_t W = 0; W < Target_Input->Keys.size(); W++)
		{
			bool New_Value = glfwGetKey(Window, Target_Input->Keys[W].Keycode) == GLFW_PRESS;
			Target_Input->Keys[W].Changed = New_Value != Target_Input->Keys[W].Pressed;			// If different? Set changed flag to 'true'
			Target_Input->Keys[W].Pressed = New_Value;											// Set/clear pressed flag accordingly
		}
	}

}