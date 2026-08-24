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

//

void Demo_Control_Camera(Jaguar::JaguarEngine* Engine, float* Angle_X, float* Angle_Y)
{
	glm::vec2 Delta = 2.5f * (Engine->Inputs.Mouse.Cursor - Engine->Inputs.Mouse.Previous);
	*Angle_X += Delta.x;
	*Angle_Y += Delta.y;

	Delta = glm::vec2(sinf(*Angle_Y), cosf(*Angle_Y));	// Just reusing this variable

	Engine->Scene.Camera.Orientation = glm::vec3(0, -Delta.x, Delta.y);
	Engine->Scene.Camera.Orientation_Up = glm::vec3(0.0f, Delta.y, Delta.x);

	Delta = glm::vec2(sinf(*Angle_X), cosf(*Angle_X));

	glm::vec3 Forwards = glm::vec3(Delta.x, 0.0f, Delta.y);
	glm::vec3 Right = glm::vec3(Delta.y, 0.0f, -Delta.x);

	Engine->Scene.Camera.Orientation = glm::vec3(
		Engine->Scene.Camera.Orientation.z * Delta.x,
		Engine->Scene.Camera.Orientation.y,
		Engine->Scene.Camera.Orientation.z * Delta.y
	);

	Engine->Scene.Camera.Orientation_Up = glm::vec3(
		Engine->Scene.Camera.Orientation_Up.z * Delta.x,
		Engine->Scene.Camera.Orientation_Up.y,
		Engine->Scene.Camera.Orientation_Up.z * Delta.y
	);
	
	if (Engine->Inputs.Keys[Demo_Controls::Forwards].Pressed)
		Engine->Scene.Camera.Position += Engine->Time * Forwards;

	if (Engine->Inputs.Keys[Demo_Controls::Backwards].Pressed)
		Engine->Scene.Camera.Position -= Engine->Time * Forwards;

	if (Engine->Inputs.Keys[Demo_Controls::Left].Pressed)
		Engine->Scene.Camera.Position -= Engine->Time * Right;

	if (Engine->Inputs.Keys[Demo_Controls::Right].Pressed)
		Engine->Scene.Camera.Position += Engine->Time * Right;

	if (Engine->Inputs.Keys[Demo_Controls::Up].Pressed)
		Engine->Scene.Camera.Position.y += Engine->Time;

	if (Engine->Inputs.Keys[Demo_Controls::Down].Pressed)
		Engine->Scene.Camera.Position.y -= Engine->Time;

	//

	if (Engine->Inputs.Keys[Demo_Controls::Escape].Pressed)					// closes game
		glfwSetWindowShouldClose(Engine->Window_Info.Window, GLFW_TRUE);
}

#endif