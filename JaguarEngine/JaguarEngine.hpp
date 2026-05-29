#ifndef JAGUAR_ENGINE_WRAPPER
#define JAGUAR_ENGINE_WRAPPER

//

#ifndef DEBUG
	#define DEBUG 1
#endif

//

#include "OpenGL_Handling/OpenGL_Declarations.hpp"

namespace Jaguar
{

	struct JaguarEngine
	{
		Window_Data Window_Info;


	};

	int Init_Jaguar(JaguarEngine* Engine);

	int Init_Window(JaguarEngine* Engine, int Width = 640, int Height = 480, const char* Name = "JaguarEngine Window", GLFWmonitor* Fullscreen = nullptr);

	void End_Jaguar(JaguarEngine* Engine);
}

#endif