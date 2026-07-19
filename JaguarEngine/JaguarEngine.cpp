#include "JaguarEngine.hpp"

#include<fstream>

namespace Jaguar
{
	//std::string Load_File_Contents(const char* Filename)

	glm::mat4 Get_View_Matrix(glm::vec3 Position, glm::vec3 Forward, glm::vec3 Up)
	{
		glm::vec3 Right = glm::cross(Up, Forward);

		return 
			glm::translate(glm::mat4(
				Right.x, Right.y, Right.z, 0.0f,
				Up.x, Up.y, Up.z, 0.0f,
				-Forward.x, -Forward.y, -Forward.z, 0.0f,
				//-Position.x, -Position.y, Position.z, 1.0f
				0.0f, 0.0f, 0.0f, 1.0f
			), -Position
			);
	}

	glm::mat4 Get_Matrix(glm::vec3 Position, glm::vec3 Forward, glm::vec3 Up)
	{
		glm::vec3 Right = glm::cross(Up, Forward);

		return glm::mat4(
			Right.x, Right.y, Right.z, 0.0f,
			Up.x, Up.y, Up.z, 0.0f,
			-Forward.x, -Forward.y, -Forward.z, 0.0f,
			Position.x, Position.y, Position.z, 1.0f
		);
	}

	std::vector<char> Load_File_Contents(const char* Directory, bool Is_Binary) // We'll use this unsigned char instead of a string so that this can handle binaries
	{
		std::ifstream File(Directory, std::ios::binary | std::ios::in); // Start at end of file

		if (!File.is_open())
		{
			printf(" >> Fatal error! Unable to load file contents: %s\n", Directory);

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

		if (!Is_Binary)
			Contents.push_back(NULL);

		return Contents;
	}

	int Init_Jaguar(JaguarEngine* Engine)
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

		glfwSetTime(0);
		Engine->Last_Time = glfwGetTime();

		return 0;
	}

	int Init_Window(JaguarEngine* Engine, int Width, int Height, const char* Name, GLFWmonitor* Fullscreen)
	{
		Engine->Window_Info.Window = glfwCreateWindow(Width, Height, Name, Fullscreen, NULL);

		if (Engine->Window_Info.Window == nullptr)
		{
			printf(" >> Failed to initialise OpenGL window!\n");
			return -1;
		}

		glfwMakeContextCurrent(Engine->Window_Info.Window);

		if (!gladLoadGL())
		{
			printf(" >> Failed to initialise glad!\n");
			return -2;
		}

		glfwGetWindowSize(Engine->Window_Info.Window, &Engine->Window_Info.Width, &Engine->Window_Info.Height);

		glViewport(0, 0, Width, Height);

		// If opened in fullscreen mode, there's no guarantee that the window matches the resolution requested.

		return 0;
	}

	void End_Jaguar(JaguarEngine* Engine)
	{
		// This will clean up the engine resources before closing the program

		glfwTerminate();
	}

	void Tick(JaguarEngine* Engine)
	{
		double Current_Time = glfwGetTime();
		double Delta_Time = Current_Time - Engine->Last_Time;
		if (Delta_Time < 0.1f)	// check lag spike
			Engine->Time = Current_Time - Engine->Last_Time;
		else
			Engine->Time = 0.04; // increment very small amount (approximately 1/25th of a second)
		Engine->Last_Time = Current_Time;
	}
}