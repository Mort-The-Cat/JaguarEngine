#ifndef JAGUAR_ENGINE_WRAPPER
#define JAGUAR_ENGINE_WRAPPER

#include "../OpenGL_Handling/OpenGL_Handling.h"
#include "../OpenGL_Handling/Input_Handling.h"
#include "../OpenGL_Handling/Scene.h"
#include "../OpenGL_Handling/Render_Queue.h"

namespace Jaguar
{

	struct Jaguar_Engine
	{
		float Time; // This is the engine's delta-time

		GLFWwindow* Window;

		Input_Data User_Inputs;

		Scene_Data Scene;

		Render_Pipeline Pipeline;	// This is the pipeline for drawing regular scene objects such as buildings or characters
									// We'll use a different interface for particles etc
	};
}

#endif