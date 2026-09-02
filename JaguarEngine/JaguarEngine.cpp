#include "JaguarEngine.hpp"

#include<fstream>
#include<uchar.h>

// https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#cats=Bit%2525252525252525252525252525252525252525252525252525252525252525252525252520Manipulation&ig_expand=4907

namespace Jaguar
{
	glm::mat4 Get_View_Matrix(glm::vec3 Position, glm::vec3 Forward, glm::vec3 Up)
	{
		glm::vec3 Right = glm::cross(Up, Forward);

		glm::mat4 View = glm::mat4(
			Right.x, Up.x, -Forward.x, 0.0f,
			Right.y, Up.y, -Forward.y, 0.0f,
			Right.z, Up.z, -Forward.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		return glm::translate(View, -Position);
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

	int Multibyte_To_UTF32(uint32_t* Character, const unsigned char* Read)
	{
		const uint32_t Masks[5] =
		{
			0x7F000000,
			0x00,		// Error!
			0x1F3F0000,
			0x0F3F3F00,
			0x073F3F3F
		};

		const uint32_t Count[5] =
		{
			1,
			0,
			2,
			3,
			4
		};

		if (!Read[0])
			return 0;

		//

		uint32_t Word = (Read[0] << 24) | (Read[1] << 16) | (Read[2] << 8) | Read[3];

		uint32_t Number = _lzcnt_u32( (~Word) );

		*Character = _pext_u32(Word, Masks[Number]);

		return Count[Number];
	}

	std::u32string Convert_UTF8(const char* Bytes)
	{
		std::u32string Output = U"";
		mbstate_t State{};
		//const char* Read = Bytes.data();
		char32_t Character;
		int Count;

		for (
			const unsigned char* Read = (const unsigned char*)Bytes;
			(Count = Multibyte_To_UTF32((uint32_t*)&Character, Read)) > 0;	// use my own function because mbrtoc32 doesn't work
			Read += Count
			)
			Output.push_back(Character);

		return Output;
	}

	std::u32string Load_UTF8_File_Contents(const char* Filename)	// This will load UTF8 files into a UTF32 string
	{
		std::vector<char> Bytes = Load_File_Contents(Filename);

		return Convert_UTF8(Bytes.data());
	}

	std::vector<char> Load_File_Contents(const char* Filename, bool Is_Binary) // We'll use this unsigned char instead of a string so that this can handle binaries
	{
		std::ifstream File(Filename, std::ios::binary | std::ios::in); // Start at end of file

		if (!File.is_open())
		{
			printf(" >> Fatal error! Unable to load file contents: %s\n", Filename);

			return std::vector<char>(); // nothing to return...
		}

		// Otherwise, we can continue

		File.seekg(0, std::ios::end);

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

		glfwSetWindowUserPointer(Engine->Window_Info.Window, Engine);	// Sets Window user-pointer.
		glfwSetCharCallback(Engine->Window_Info.Window, Callback_Read_Text_Input);		// For all other keypresses
		glfwSetKeyCallback(Engine->Window_Info.Window, Callback_Read_Special_Input);	// For enter/backspace/tab pressed

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