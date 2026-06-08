#include "JaguarEngine.hpp"

#include<fstream>

namespace Jaguar
{
	//std::string Load_File_Contents(const char* Filename)
	std::vector<char> Load_File_Contents(const char* Filename) // We'll use this unsigned char instead of a string so that this can handle binaries
	{
		std::ifstream File(Filename, std::ios::binary); // Start at end of file

		if (!File.is_open())
		{
			printf(" >> Fatal error! Unable to load file contents: %s\n", Filename);

			return std::vector<char>(); // nothing to return...
		}

		// Otherwise, we can continue

		File.seekg(0, std::ios::end);

		//std::string Contents;
		std::vector<char> Contents;
		size_t Size = File.tellg();

		File.seekg(0, std::ios::beg);

		Contents.resize(Size); // Fits buffer

		File.read((char*)Contents.data(), Size);

		File.close();

		return Contents;
	}

	int Init_Jaguar(JaguarEngine* Engine)
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

		if (gladLoadGL())
		{
			printf(" >> Failed to initialise glad!\n");
			return 1;
		}

		return 0;
	}

	int Init_Window(JaguarEngine* Engine, int Width, int Height, const char* Name, GLFWmonitor* Fullscreen)
	{
		Engine->Window_Info.Window = glfwCreateWindow(Width, Height, Name, Fullscreen, NULL);

		if (Engine->Window_Info.Window == nullptr)
		{
			printf(" >> Failed to initialise OpenGL window!\n");
			return 1;
		}

		glfwMakeContextCurrent(Engine->Window_Info.Window);

		glfwGetWindowSize(Engine->Window_Info.Window, &Engine->Window_Info.Width, &Engine->Window_Info.Height);

		// If opened in fullscreen mode, there's no guarantee that the window matches the resolution requested.

		return 0;
	}

	void End_Jaguar(JaguarEngine* Engine)
	{
		// This will clean up the engine resources before closing the program

		glfwTerminate();
	}
}