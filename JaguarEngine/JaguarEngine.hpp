#ifndef JAGUAR_ENGINE_WRAPPER
#define JAGUAR_ENGINE_WRAPPER

//

#ifndef DEBUG
	#define DEBUG 0
#endif

//

#include<string>
#include "OpenGL_Handling/OpenGL_Declarations.hpp"

#include "JSON_IO/JSON_Declarations.hpp"

namespace Jaguar
{


	struct JaguarEngine
	{
		Window_Data Window_Info;


	};

	int Init_Jaguar(JaguarEngine* Engine);

	int Init_Window(JaguarEngine* Engine, int Width = 640, int Height = 480, const char* Name = "JaguarEngine Window", GLFWmonitor* Fullscreen = nullptr);

	void End_Jaguar(JaguarEngine* Engine);

	std::vector<char> Load_File_Contents(const char* Filename);
}

#endif